#pragma once
#include <vector>
#include <string>
#include <glm/vec3.hpp>

#include "BxDF.h"
#include "Pixel.h"
#include "Coordinate_frame.h"


namespace Raytracer
{
  namespace Util
  {
    void split(const std::string& s, const char* delim, std::vector<std::string>& v);
    BxDF::Type get_material_type(const std::string& type);
    // TODO: move to sampler
    glm::vec3 sample_hemisphere(const float x0, const float x1);
    glm::vec3 sample_cosine_weighted_hemisphere(const float x0, const float x1);
    glm::vec3 sample_sphere(const float x0, const float x1);
    float fresnel_schlick(const float cos_theta,
                          const float eta_0,
                          const float eta_1);
    float fresnel(const float cos_theta,
                  const float eta_0,
                  const float eta_1);
    glm::vec3 world_to_surface(const Coordinate_frame& ONB, const glm::vec3& v);
    glm::vec3 surface_to_world(const Coordinate_frame& ONB, const glm::vec3& v);;
    glm::vec3 max_colour(std::vector<Pixel>& pixelbuffer);
    float max_depth(std::vector<Pixel>& pixelbuffer);
  }
}