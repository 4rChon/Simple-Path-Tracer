#pragma once

#include <glm/vec3.hpp>
#include "Coordinate_frame.h"
#include "Ray.h"

namespace Raytracer {
class Camera {
  protected:
    CoordinateFrame F_;
    const float distance_;
    const float v_h_;
    const float v_w_;

  public:
    enum Type {
        Pinhole,
        Lens
    };

    Camera(const float fov, const float aspect_ratio, const float distance,
           const glm::vec3& position, const glm::vec3& target);
    virtual ~Camera() = 0;
    virtual Ray get_ray(const float x, const float y);
    virtual Ray get_ray(const float x, const float y, const glm::vec2& jitter);
    virtual Type get_type() const = 0;
};
} // namespace Raytracer