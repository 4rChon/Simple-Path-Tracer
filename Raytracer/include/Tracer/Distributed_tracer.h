#pragma once

#include <array>
#include "glm/vec3.hpp"
#include "Ray.h"
#include "Sampler.h"
#include "Scene.h"
#include "Tracer.h"

namespace Raytracer {
class DistributedTracer : public Tracer {
  private:
    const unsigned int reflection_samples_ = 32;
    const unsigned int transmission_samples_ = 1;
    const unsigned int shadow_samples_ = 32;

  public:
    DistributedTracer(std::array<unsigned int, 2> dimensions, unsigned int depth);
    ~DistributedTracer() override;

    glm::vec3 trace(Scene& scene, Ray& ray, Sampler& sampler, unsigned int depth) override;
};
} // namespace Raytracer