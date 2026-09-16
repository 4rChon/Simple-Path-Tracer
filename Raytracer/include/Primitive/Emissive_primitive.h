#pragma once

#include "ILight.h"
#include "IMaterial.h"
#include "Intersection.h"
#include "IPrimitive.h"
#include "IShape.h"
#include "Ray.h"

namespace Raytracer {
class EmissivePrimitive : public IPrimitive {
  private:
    IShape* shape_;
    IMaterial* material_;
    ILight* light_;

  public:
    EmissivePrimitive(IShape& shape, IMaterial& material, ILight& light);
    ~EmissivePrimitive() override;

    bool intersects(const Ray& ray, Intersection& i_out);
    bool intersects(const Ray& ray);
};
} // namespace Raytracer