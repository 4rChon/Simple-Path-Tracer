#include <cmath>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <stdexcept>
#include "Camera.h"
#include "Ray.h"

namespace Raytracer {
Camera::Camera(const float fov, const float aspect_ratio, const float distance,
               const glm::vec3& position, const glm::vec3& target)
    : v_h_(tan(fov * 0.5f)), v_w_(v_h_ * aspect_ratio), distance_(distance / 3.7070F)
{
    F_.look_at(position, target, glm::vec3(0, 1, 0));
}

Camera::~Camera() {}

Ray Camera::get_ray(const float, const float)
{
    throw std::logic_error("Not implemented.");
}

Ray Camera::get_ray(const float, const float, const glm::vec2&)
{
    throw std::logic_error("Not implemented.");
}
} // namespace Raytracer