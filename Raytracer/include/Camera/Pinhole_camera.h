#pragma once

#include <glm/vec3.hpp>
#include "Camera.h"
#include "Ray.h"

namespace Raytracer {
class PinholeCamera : public Camera {
  public:
    PinholeCamera(const float fov, const float aspect_ratio, const float distance,
                   const glm::vec3& position, const glm::vec3& target);

    ~PinholeCamera() override;

    Ray get_ray(const float x, const float y) override;
    Type get_type() const override
    {
        return Camera::Pinhole;
    }
};
} // namespace Raytracer