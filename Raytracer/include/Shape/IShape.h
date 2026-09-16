#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include "Intersection.h"
#include "Ray.h"

namespace Raytracer {
class IShape {
  public:
    IShape();
    virtual ~IShape() = 0;

    // Test also populates Intersection structure
    virtual bool intersects(const Ray& ray, Intersection& i_out) = 0;
    // Test simply returns true/false
    virtual bool intersects(const Ray& ray) = 0;
    // Surface area of shape
    virtual float get_surface_area() const = 0;
    // Get a surface coordinate given an offset
    virtual glm::vec3 sample_surface(const std::vector<float>& xi) const = 0;
    // Get projected surface area
    virtual float get_projected_area() const = 0;
    // Get point normal
    virtual glm::vec3 surface_normal(const glm::vec3& point) const = 0;
};
} // namespace Raytracer