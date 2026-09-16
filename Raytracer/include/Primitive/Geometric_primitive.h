#pragma once

#include "IMaterial.h"
#include "Intersection.h"
#include "IPrimitive.h"
#include "IShape.h"
#include "Ray.h"

namespace Raytracer {
class GeometricPrimitive : public IPrimitive {
  private:
    IShape* shape_;
    IMaterial* material_;

  public:
    GeometricPrimitive(IShape& shape, IMaterial& material);
    ~GeometricPrimitive() override;

    bool intersects(const Ray& ray, Intersection& i_out);
    bool intersects(const Ray& ray);
};
} // namespace Raytracer