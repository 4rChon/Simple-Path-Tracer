#pragma once

#include <algorithm>
#include <glm/gtx/norm.hpp>
#include <glm/vec3.hpp>
#include "Spectrum.h"

namespace Raytracer::Spectrum {
bool is_black(const glm::vec3& brdf)
{
    return glm::length2(brdf) == 0.f;
}

float max_component(const glm::vec3& v)
{
    return std::max(std::max(v.r, v.g), v.b);
}
} // namespace Raytracer::Spectrum
