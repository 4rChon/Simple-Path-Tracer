#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "Camera.h"
#include "Ray.h"

namespace Raytracer {
class LensCamera : public Camera {
  private:
    const float f_p_;
    const float alpha_;

  public:
    LensCamera(const float fov, const float aspect_ratio, const float distance,
                const glm::vec3& position, const glm::vec3& target, const float f_p,
                const float alpha);

    ~LensCamera() override;

    Ray get_ray(const float x, const float y, const glm::vec2& jitter) override;
    Type get_type() const override
    {
        return Camera::Lens;
    }
};
} // namespace Raytracer