#include <glm/vec3.hpp>
#include "Ray.h"

namespace Raytracer {
Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) : origin_(origin), direction_(direction) {}

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction, const float clipping_distance[2])
    : origin_(origin), direction_(direction), clip_min(clipping_distance[0]), clip_max(clipping_distance[1])
{
}

void Ray::set(const glm::vec3& origin, const glm::vec3& direction)
{
    this->origin_ = origin;
    this->direction_ = direction;
}
} // namespace Raytracer