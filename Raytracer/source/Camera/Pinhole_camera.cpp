#include <glm/gtx/norm.hpp>
#include <glm/vec3.hpp>
#include "Camera.h"
#include "Pinhole_camera.h"
#include "Ray.h"

namespace Raytracer {
PinholeCamera::PinholeCamera(const float fov, const float aspect_ratio,
                               const float distance, const glm::vec3& position,
                               const glm::vec3& target)
    : Camera(fov, aspect_ratio, distance, position, target)
{
}

PinholeCamera::~PinholeCamera() {}

Ray PinholeCamera::get_ray(const float x, const float y)
{
    glm::vec3 C = F_.W * distance_ - F_.U * v_w_ - F_.V * v_h_;
    glm::vec3 D = glm::normalize(C + 2.f * (F_.U * x * v_w_ + F_.V * y * v_h_));

    return Ray(F_.P, D);
}
} // namespace Raytracer