#include <algorithm>
#include <array>
#include <fstream>
#include <glm/vec3.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "Ad_hoc_material.h"
#include "Area_light.h"
#include "BxDF.h"
#include "Camera.h"
#include "Emissive_primitive.h"
#include "Geometric_primitive.h"
#include "ILight.h"
#include "IMaterial.h"
#include "IShape.h"
#include "json.h"
#include "Lens_camera.h"
#include "Path_tracer.h"
#include "Pinhole_camera.h"
#include "Point_light.h"
#include "Sampler.h"
#include "Scene.h"
#include "Sphere.h"
#include "Tracer.h"
#include "util.h"
#include "Whitted_tracer.h"

namespace Raytracer::Json {
namespace {
glm::vec3 parse_vec3(const json& value)
{
    if (!value.is_array() || value.size() != 3) {
        throw std::runtime_error("Expected JSON array containing exactly 3 elements");
    }

    return {value.at(0).get<float>(), value.at(1).get<float>(), value.at(2).get<float>()};
}
} // namespace

json parse_json_document(const std::string& path)
{
    std::cout << "Parsing json file: " << path << '\n';

    std::ifstream file(path);

    if (!file) {
        throw std::runtime_error("Could not open JSON file: " + path);
    }

    json document;

    try {
        file >> document;
    }
    catch (const json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << '\n';

        throw;
    }

    std::cout << "Json parsed: " << path << '\n';

    return document;
}

const json& get_element_array(const json& document, const char* element)
{
    const auto& value = document.at(element);

    if (!value.is_array()) {
        throw std::runtime_error(std::string(element) + " is not a JSON array");
    }

    return value;
}

std::unordered_map<std::string, Camera*> init_cameras(const json& document)
{
    const auto& cameras = get_element_array(document, "cameras");

    std::unordered_map<std::string, Camera*> scene_cameras;

    for (const auto& camera : cameras) {
        const std::string id = camera.at("id").get<std::string>();

        const std::string type = camera.at("type").get<std::string>();

        const float fov = camera.at("fov").get<float>();

        const float aspect = camera.at("aspect").get<float>();

        const float distance = camera.at("distance").get<float>();

        const glm::vec3 position = parse_vec3(camera.at("position"));

        const glm::vec3 target = parse_vec3(camera.at("target"));

        if (type == "pinhole") {
            Camera* camera_entity =
                new PinholeCamera(fov, aspect, distance, position, target);

            scene_cameras.emplace(id, camera_entity);
        }
        else if (type == "lens-based") {
            const float fp = camera.at("focal distance").get<float>();

            const float alpha = camera.at("alpha").get<float>();

            Camera* camera_entity =
                new LensCamera(fov, aspect, distance, position, target, fp, alpha);

            scene_cameras.emplace(id, camera_entity);
        }
        else {
            throw std::runtime_error("Unknown camera type: " + type);
        }
    }

    return scene_cameras;
}

std::unordered_map<std::string, IMaterial*> init_materials(const json& document)
{
    const auto& materials = get_element_array(document, "materials");

    std::unordered_map<std::string, IMaterial*> scene_materials;

    for (const auto& json_material : materials) {
        const std::string id = json_material.at("id").get<std::string>();
        const std::string types = json_material.at("type").get<std::string>();

        std::vector<std::string> type_list = Util::split(types, " ");

        int material_type = 0;

        glm::vec3 rho_d{0.0F};
        glm::vec3 rho_r{0.0F};
        glm::vec3 rho_t{0.0F};

        float beta = 0.0F;
        float eta = 0.0F;

        if (types == "diffuse") {
            rho_d = parse_vec3(json_material.at("rho"));

            material_type = BxDF::Diffuse;
        }
        else if (types == "specular reflection") {
            rho_r = parse_vec3(json_material.at("rho"));

            material_type = BxDF::Specular | BxDF::Reflect;
        }
        else if (types == "glossy reflection") {
            rho_r = parse_vec3(json_material.at("rho"));

            rho_d = rho_r;

            beta = json_material.at("roughness").get<float>();

            material_type = BxDF::Glossy | BxDF::Reflect | BxDF::Specular;
        }
        else if (types == "fresnel dielectric") {
            rho_r = parse_vec3(json_material.at("rhoR"));

            rho_t = parse_vec3(json_material.at("rhoT"));

            eta = json_material.at("eta").get<float>();

            material_type = BxDF::Specular | BxDF::Reflect | BxDF::Transmit;
        }
        else {
            throw std::runtime_error("Unknown material type: " + types);
        }

        IMaterial* material =
            new AdHocMaterial(material_type, rho_d, rho_r, rho_t, beta, eta);

        scene_materials.emplace(id, material);
    }

    return scene_materials;
}

std::unordered_map<std::string, IShape*> init_shapes(const json& document)
{
    const auto& shapes = get_element_array(document, "shapes");

    std::unordered_map<std::string, IShape*> scene_shapes;

    for (const auto& shape : shapes) {
        const std::string id = shape.at("id").get<std::string>();

        const std::string type = shape.at("type").get<std::string>();

        const glm::vec3 centre = parse_vec3(shape.at("centre"));

        const float radius = shape.at("radius").get<float>();

        if (type == "sphere") {
            IShape* shape_entity = new Sphere(radius, centre);

            scene_shapes.emplace(id, shape_entity);
        }
        else {
            throw std::runtime_error("Unknown shape type: " + type);
        }
    }

    return scene_shapes;
}

std::unordered_map<std::string, ILight*> init_lights(const json& document)
{
    const auto& lights = get_element_array(document, "lights");

    std::unordered_map<std::string, ILight*> scene_lights;

    for (const auto& light : lights) {
        const std::string id = light.at("id").get<std::string>();

        const std::string type = light.at("type").get<std::string>();

        const glm::vec3 power = parse_vec3(light.at("power"));

        if (type == "point") {
            const glm::vec3 position = parse_vec3(light.at("position"));

            ILight* light_entity = new PointLight(position, power);

            scene_lights.emplace(id, light_entity);
        }
        else if (type == "area") {
            auto shapes = init_shapes(document);

            const std::string shape_id = light.at("shape").get<std::string>();

            assert_exists(shapes, shape_id.c_str(), "shape");

            ILight* light_entity = new AreaLight(*shapes.at(shape_id), power);

            scene_lights.emplace(id, light_entity);
        }
        else {
            throw std::runtime_error("Unknown light type: " + type);
        }
    }

    return scene_lights;
}

Scene* get_scene(const json& document)
{
    const auto& json_scene = document.at("scene");

    auto materials = init_materials(document);

    auto shapes = init_shapes(document);

    auto lights = init_lights(document);

    const auto& light_ids = get_element_array(json_scene, "lights");

    const auto& primitive_list = get_element_array(json_scene, "primitives");

    Scene* scene = new Scene();

    for (const auto& primitive : primitive_list) {
        const std::string id = primitive.at("id").get<std::string>();

        const std::string type = primitive.at("type").get<std::string>();

        const std::string shape_id = primitive.at("shape").get<std::string>();

        const std::string material_id = primitive.at("material").get<std::string>();

        assert_exists(shapes, shape_id.c_str(), "shape");

        assert_exists(materials, material_id.c_str(), "material");

        if (type == "geometric") {
            auto* p =
                new GeometricPrimitive(*shapes.at(shape_id), *materials.at(material_id));

            scene->add_primitive(*p);
        }
        else if (type == "emissive") {
            const std::string light_id = primitive.at("light").get<std::string>();

            assert_exists(lights, light_id.c_str(), "light");

            auto* p = new EmissivePrimitive(
                *shapes.at(shape_id), *materials.at(material_id), *lights.at(light_id));

            scene->add_primitive(*p);
        }
        else {
            throw std::runtime_error("Unknown primitive type: " + type);
        }
    }

    for (const auto& light_id_json : light_ids) {
        const std::string light_id = light_id_json.get<std::string>();

        assert_exists(lights, light_id.c_str(), "light");

        scene->add_light_source(*lights.at(light_id));
    }

    return scene;
}

Sampler* get_sampler(const json& document)
{
    const auto& json_scene = document.at("scene");

    const auto& renderer = json_scene.at("renderer");

    const int samples = renderer.at("samples").get<int>();

    return new Sampler(samples);
}

Camera* get_camera(const json& document)
{
    const auto& json_scene = document.at("scene");

    auto cameras = init_cameras(document);

    const std::string camera_id = json_scene.at("camera").get<std::string>();

    assert_exists(cameras, camera_id.c_str(), "camera");

    return cameras.at(camera_id);
}

Tracer* get_renderer(const json& document)
{
    const auto& json_scene = document.at("scene");

    const auto& renderer = json_scene.at("renderer");

    const std::string renderer_type = renderer.at("type").get<std::string>();

    const auto& json_dims = get_element_array(renderer, "dimensions");

    if (json_dims.size() != 2) {
        throw std::runtime_error("Renderer dimensions must contain exactly 2 values");
    }

    std::array<unsigned int, 2> dimensions{json_dims.at(0).get<unsigned int>(),
                                           json_dims.at(1).get<unsigned int>()};

    int depth = renderer.at("depth").get<int>();
    depth = std::min(depth, 20);

    if (renderer_type == "WRT") {
        return new Whitted_tracer(dimensions, static_cast<unsigned int>(depth));
    }

    if (renderer_type == "PT") {
        return new Path_tracer(dimensions, static_cast<unsigned int>(depth));
    }

    throw std::runtime_error("Unknown tracer type: " + renderer_type);
}
} // namespace Raytracer::Json
