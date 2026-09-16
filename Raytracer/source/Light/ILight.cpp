#include <glm/vec3.hpp>
#include "ILight.h"

namespace Raytracer {
ILight::ILight(const glm::vec3& L_e) : L_e_(L_e) {}

ILight::~ILight() {}
} // namespace Raytracer