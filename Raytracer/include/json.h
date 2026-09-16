#pragma once

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

#include "Camera.h"
#include "ILight.h"
#include "IMaterial.h"
#include "IShape.h"
#include "Sampler.h"
#include "Scene.h"
#include "Tracer.h"

namespace fs = std::filesystem;

namespace Raytracer::Json {
using json = nlohmann::json;

json parse_json_document(const fs::path& path);

const json& get_element_array(const json& document, const char* element);

std::unordered_map<std::string, Camera*> init_cameras(const json& document);
std::unordered_map<std::string, IMaterial*> init_materials(const json& document);
std::unordered_map<std::string, IShape*> init_shapes(const json& document);
std::unordered_map<std::string, ILight*> init_lights(const json& document);

Scene* get_scene(const json& document);
Sampler* get_sampler(const json& document);
Camera* get_camera(const json& document);
Tracer* get_renderer(const json& document);

template <typename T>
void assert_exists(const std::unordered_map<std::string, T>& map, const char* key,
                   const char* object_type)
{
    if (map.find(key) == map.end()) {
        std::cout << key << " is not a valid " << object_type << " object.\n";
        std::exit(EXIT_FAILURE);
    }
}
} // namespace Raytracer::Json