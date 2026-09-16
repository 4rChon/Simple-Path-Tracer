#include <cmath>
#include <glm/gtx/norm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <numbers>
#include "Camera.h"
#include "Lens_camera.h"
#include "Ray.h"

namespace Raytracer {
LensCamera::LensCamera(const float fov, const float aspect_ratio, const float distance,
                       const glm::vec3& position, const glm::vec3& target,
                       const float f_p, const float alpha)
    : Camera(fov, aspect_ratio, distance, position, target), f_p_(f_p), alpha_(alpha)
{
}

LensCamera::~LensCamera() {}
Ray LensCamera::get_ray(const float x, const float y, const glm::vec2& jitter)
{
    float theta = (float)(std::numbers::pi_v<float> * 2 * jitter[0]);
    float r = alpha_ * sqrt(jitter[1]) * 0.5f;
    float r_cos_theta = r * cos(theta);
    float r_sin_theta = r * sin(theta);

    glm::vec3 C = F_.W * distance_ - F_.U * v_w_ - F_.V * v_h_;
    glm::vec3 D = glm::normalize(C + 2.f * (F_.U * x * v_w_ + F_.V * y * v_h_));
    glm::vec3 P_f = D * (1.f + f_p_);
    glm::vec3 P_j = D + F_.U * r_cos_theta + F_.V * r_sin_theta;
    glm::vec3 D_v = (P_f - P_j) / f_p_;

    return Ray(F_.P + P_j - D_v, glm::normalize(D_v), {1e-5f, 1e5f});
}
} // namespace Raytracer