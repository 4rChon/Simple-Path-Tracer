#pragma once

#include <array>
#include <glm/vec3.hpp>

constexpr auto clip_min_epsilon = 0.001f;

namespace Raytracer {
struct Ray {
    glm::vec3 origin_{0.0f};
    glm::vec3 direction_{0.0f};
    float clip_min = clip_min_epsilon;
    float clip_max = 10000.f;

    Ray() = default;

    Ray(const glm::vec3& O, const glm::vec3& D,
        const std::array<float, 2> clipping_distance);
    Ray(const glm::vec3& O, const glm::vec3& D);
    void set(const glm::vec3& O, const glm::vec3& D);
};
} // namespace Raytracer