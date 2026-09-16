#include "Geometric_primitive.h"
#include "IMaterial.h"
#include "Intersection.h"
#include "IShape.h"
#include "Ray.h"

namespace Raytracer {
GeometricPrimitive::GeometricPrimitive(IShape& shape, IMaterial& material)
    : shape_(&shape), material_(&material)
{
}
GeometricPrimitive::~GeometricPrimitive() {}

bool GeometricPrimitive::intersects(const Ray& ray, Intersection& i_out)
{
    if (shape_->intersects(ray, i_out)) {
        i_out.has_light = false;
        i_out.material = material_;
    }

    return i_out.is_valid;
}

bool GeometricPrimitive::intersects(const Ray& ray)
{
    return shape_->intersects(ray);
}
} // namespace Raytracer