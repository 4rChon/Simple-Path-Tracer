#pragma once

#include <glm/vec3.hpp>
#include <string>
#include <vector>
#include "BxDF.h"
#include "Coordinate_frame.h"
#include "Pixel.h"

namespace Raytracer::Util {
std::vector<std::string> split(std::string_view input, std::string_view delimiters);
BxDF::Type get_material_type(const std::string& type);
// TODO: move to sampler
glm::vec3 sample_hemisphere(const float x0, const float x1);
glm::vec3 sample_cosine_weighted_hemisphere(const float x0, const float x1);
glm::vec3 sample_sphere(const float x0, const float x1);
float fresnel_schlick(const float cos_theta, const float eta_0, const float eta_1);
float fresnel(const float cos_theta, const float eta_0, const float eta_1);
glm::vec3 world_to_surface(const CoordinateFrame& ONB, const glm::vec3& v);
glm::vec3 surface_to_world(const CoordinateFrame& ONB, const glm::vec3& v);
;
glm::vec3 max_colour(const std::vector<Pixel>& pixelbuffer);
float max_depth(const std::vector<Pixel>& pixelbuffer);
} // namespace Raytracer::Util
