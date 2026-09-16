#include "Intersection.h"

namespace Raytracer {
Intersection::Intersection() {}

Intersection& Intersection::operator=(const Intersection& rvalue)
{
    this->is_valid = rvalue.is_valid;
    this->has_light = rvalue.has_light;
    this->distance = rvalue.distance;
    this->L_e = rvalue.L_e;
    this->D = rvalue.D;
    this->P = rvalue.P;
    this->geometry_ONB = rvalue.geometry_ONB;
    this->shading_ONB = rvalue.shading_ONB;
    this->shape = rvalue.shape;
    this->material = rvalue.material;

    return *this;
}
} // namespace Raytracer