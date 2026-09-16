#pragma once

#include <array>
#include "glm/vec3.hpp"
#include "Ray.h"
#include "Sampler.h"
#include "Scene.h"
#include "Tracer.h"

namespace Raytracer {
class Path_tracer : public Tracer {
  public:
    Path_tracer(std::array<unsigned int, 2> dimensions, unsigned int depth);
    ~Path_tracer();

    glm::vec3 trace(Scene& scene, Ray& ray, Sampler& sampler, unsigned int depth) override;
};
} // namespace Raytracer