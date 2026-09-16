#include <filesystem>
#include <gflags/gflags.h>
#include <nlohmann/json.hpp>
#include <string>
#include <sys/stat.h>
#include <vector>
#include "Camera.h"
#include "json.h"
#include "Sampler.h"
#include "Scene.h"
#include "Tracer.h"

namespace fs = std::filesystem;
using json = nlohmann::json;

static bool validate_load_path(const char* flagname, const std::string& value);
static bool validate_save_path(const char* flagname, const std::string& value);
static bool validate_input_file(const char* flagname, const std::string& value);
static bool validate_tonemapper(const char* flagname, const std::string& value);

DEFINE_bool(batch, true, "Render all files in the load folder");

DEFINE_string(load_path, "./definitions/", "Folder containing scene definition file.");
DEFINE_validator(load_path, &validate_load_path);

DEFINE_string(save_path, "./renders/", "Save folder location.");
DEFINE_validator(save_path, &validate_save_path);

DEFINE_string(input_file, "in.json", "Input filename");
DEFINE_validator(input_file, &validate_input_file);

DEFINE_string(ppm_output_file, "out.ppm", "Output filename");
DEFINE_string(jpg_output_file, "out.jpg", "Output filename");
DEFINE_bool(draw, true, "Draw result after tracing is complete");
DEFINE_bool(depth_map, false, "Output a depth map instead of the coloured render");

DEFINE_string(tonemapper, "sigmoid", "Tone mapper to use (sigmoid | linear");
DEFINE_validator(tonemapper, &validate_tonemapper);

static bool validate_load_path(const char*, const std::string& value)
{
    struct stat buffer;

    if (stat(value.c_str(), &buffer) != 0) {
        return false;
    }
    else if (buffer.st_mode & S_IFDIR) {
        return true;
    }
    return false;
}

static bool validate_save_path(const char*, const std::string& value)
{
    if (!fs::create_directory(fs::directory_entry(value))) {
        return true;
    }

    struct stat buffer;

    if (stat(value.c_str(), &buffer) != 0) {
        return false;
    }
    else if (buffer.st_mode & S_IFDIR) {
        return true;
    }
    return false;
}

static bool validate_input_file(const char*, const std::string& value)
{
    if (FLAGS_batch) {
        return true;
    }

    struct stat buffer;
    std::string full_path = FLAGS_load_path + value;
    return stat(full_path.c_str(), &buffer) == 0;
}

static bool validate_tonemapper(const char*, const std::string& value)
{
    return value.compare("sigmoid") == 0 || value.compare("linear") == 0 ||
           value.compare("none") == 0;
}

int main(int argc, char* argv[])
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    std::vector<fs::path> input_paths;
    std::vector<fs::path> ppm_output_paths;
    std::vector<fs::path> jpg_output_paths;
    fs::path load_path(FLAGS_load_path);
    fs::path save_path(FLAGS_save_path);

    if (FLAGS_batch) {
        for (const auto& p : fs::directory_iterator(FLAGS_load_path)) {
            input_paths.push_back(p.path());

            fs::path ppm_save_file(p.path().filename());
            ppm_save_file.replace_extension("ppm");
            ppm_output_paths.push_back(save_path / ppm_save_file);

            fs::path jpg_save_file(p.path().filename());
            jpg_save_file.replace_extension("jpg");
            jpg_output_paths.push_back(save_path / jpg_save_file);
        }
    }
    else {
        input_paths.push_back(load_path / FLAGS_input_file);
        ppm_output_paths.push_back(save_path / FLAGS_ppm_output_file);
        jpg_output_paths.push_back(save_path / FLAGS_jpg_output_file);
    }

    unsigned int i = 0;
    for (auto& path : input_paths) {
        json document = Raytracer::Json::parse_json_document(path);
        Raytracer::Scene* scene = Raytracer::Json::get_scene(document);
        Raytracer::Camera* camera = Raytracer::Json::get_camera(document);
        Raytracer::Tracer* tracer = Raytracer::Json::get_renderer(document);
        int spp = Raytracer::Json::get_spp(document);

        if (!FLAGS_depth_map) {
            tracer->render(*scene, *camera, spp);

            if (FLAGS_tonemapper.compare("sigmoid") == 0) {
                tracer->tone_map_sigmoid();
            }
            else if (FLAGS_tonemapper.compare("linear") == 0) {
                tracer->tone_map_linear();
            }
            tracer->save(ppm_output_paths[i]);
        }
        else {
            tracer->render_depth_map(*scene, *camera);
            tracer->tone_map_depth();
            tracer->save_depth(fs::path(ppm_output_paths[i]));
        }
        if (FLAGS_draw) {
            tracer->draw(fs::path(jpg_output_paths[i]));
        }

        delete scene;
        delete camera;
        delete tracer;
        ++i;
    }
}