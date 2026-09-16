#include <glm/gtx/norm.hpp>
#include <glm/vec3.hpp>
#include <numbers>
#include <vector>
#include "ILight.h"
#include "Point_light.h"

namespace Raytracer {
PointLight::PointLight(const glm::vec3& position, const glm::vec3& L_e)
    : position_(position), ILight(L_e)
{
}

PointLight::~PointLight() {}

glm::vec3 PointLight::sample_L(const glm::vec3& P, Light_sample& light_sample)
{
    glm::vec3 light_to_point = position_ - P;
    light_sample.direction = glm::normalize(light_to_point);
    light_sample.distance = glm::length(light_to_point);
    light_sample.position = position_;
    light_sample.PDF = 1.f / std::numbers::pi_v<float>;
    return L_e_ / glm::length2(light_to_point);
}

glm::vec3 PointLight::sample_L(const std::vector<float>&, const glm::vec3&, Light_sample&)
{
    throw "Not implemented.";
}
} // namespace Raytracer