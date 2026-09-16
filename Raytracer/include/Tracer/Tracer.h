#pragma once

#include <array>
#include <glm/vec3.hpp>
#include <string>
#include <vector>
#include "Camera.h"
#include "Pixel.h"
#include "Ray.h"
#include "Sampler.h"
#include "Scene.h"

namespace Raytracer {
class Tracer {
  protected:
    std::vector<Pixel> framebuffer_;
    const unsigned int width_;
    const unsigned int height_;
    const unsigned int depth_;

  public:
    Tracer(std::array<unsigned int, 2> dimensions, unsigned int depth);
    virtual ~Tracer() = 0;

    std::vector<Pixel> render(Scene& scene, Camera& camera, Sampler& sampler);

    virtual glm::vec3 trace(Scene& scene, Ray& ray, Sampler& sampler,
                            unsigned int depth) = 0;

    void render_depth_map(Scene& scene, Camera& camera);
    void tone_map_linear();
    void tone_map_sigmoid();
    void tone_map_depth();
    void save(const std::string& path, const std::string& filename) const;
    void save_depth(const std::string& path, const std::string& filename) const;
    void draw() const;
};
} // namespace Raytracer