#pragma once

#include "Intersection.h"
#include "Ray.h"

namespace Raytracer {
class IPrimitive {
  public:
    IPrimitive();
    virtual ~IPrimitive() = 0;

    virtual bool intersects(const Ray& ray, Intersection& i_out) = 0;
    virtual bool intersects(const Ray& ray) = 0;
};
} // namespace Raytracer