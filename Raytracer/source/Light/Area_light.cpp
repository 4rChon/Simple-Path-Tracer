#include <algorithm>
#include <glm/gtx/norm.hpp>
#include <glm/vec3.hpp>
#include <numbers>
#include <vector>

#include "Area_light.h"
#include "ILight.h"
#include "IShape.h"
#include "Sphere.h"

namespace Raytracer {
AreaLight::AreaLight(IShape& shape, const glm::vec3& L_e_) : shape_(&shape), ILight(L_e_)
{
}

AreaLight::~AreaLight() {}

glm::vec3 AreaLight::sample_L(const glm::vec3&, Light_sample&)
{
    throw "Not implemented.";
}

glm::vec3 AreaLight::sample_L(const std::vector<float>& xi, const glm::vec3& P,
                              Light_sample& light_sample)
{
    glm::vec3 surface_normal = shape_->surface_normal(P);
    glm::vec3 surface_point =
        ((Sphere*)shape_)
            ->sample_cosine_weighted_facing_surface(xi[0], xi[1], surface_normal);

    glm::vec3 light_surface_to_point = surface_point - P;
    light_sample.direction = -glm::normalize(light_surface_to_point);
    light_sample.distance = glm::length(light_surface_to_point);
    light_sample.position = surface_point;

    float cos_theta_y = std::max(0.f, glm::dot(light_sample.direction, surface_normal));
    light_sample.PDF =
        glm::length2(light_surface_to_point) / (std::numbers::pi_v<float> * cos_theta_y);

    return L_e_;
}

} // namespace Raytracer