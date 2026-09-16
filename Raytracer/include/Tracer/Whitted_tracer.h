#pragma once
#include <array>
#include "glm/ext/vector_float3.hpp"
#include "Ray.h"
#include "Sampler.h"
#include "Scene.h"
#include "Tracer.h"

namespace Raytracer {
class Whitted_tracer : public Tracer {
  public:
    Whitted_tracer(std::array<unsigned int, 2> dimensions, unsigned int depth);
    ~Whitted_tracer() override;

    glm::vec3 trace(Scene& scene, Ray& ray, Sampler& sampler, unsigned int depth) override;
};
} // namespace Raytracer