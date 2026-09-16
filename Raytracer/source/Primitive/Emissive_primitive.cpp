#include "Emissive_primitive.h"
#include "ILight.h"
#include "IMaterial.h"
#include "Intersection.h"
#include "IShape.h"
#include "Ray.h"

namespace Raytracer {
EmissivePrimitive::EmissivePrimitive(IShape& shape, IMaterial& material, ILight& light)
    : shape_(&shape), material_(&material), light_(&light)
{
}
EmissivePrimitive::~EmissivePrimitive() {}

bool EmissivePrimitive::intersects(const Ray& ray, Intersection& i_out)
{
    if (shape_->intersects(ray, i_out)) {
        i_out.has_light = true;
        i_out.L_e = light_->L_e();
        i_out.material = material_;
    }

    return i_out.is_valid;
}

bool EmissivePrimitive::intersects(const Ray& ray)
{
    return shape_->intersects(ray);
}
} // namespace Raytracer