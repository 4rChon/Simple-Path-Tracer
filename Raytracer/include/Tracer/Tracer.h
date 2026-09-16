#pragma once

#include <array>
#include <filesystem>
#include <glm/vec3.hpp>
#include <vector>

#include "Camera.h"
#include "Pixel.h"
#include "Ray.h"
#include "Sampler.h"
#include "Scene.h"

namespace fs = std::filesystem;

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

    std::vector<Pixel> render(Scene& scene, Camera& camera, int spp);

    virtual glm::vec3 trace(Scene& scene, Ray& ray, Sampler& sampler,
                            unsigned int depth) = 0;

    void render_depth_map(Scene& scene, Camera& camera);
    void tone_map_linear();
    void tone_map_sigmoid();
    void tone_map_depth();
    void save(const fs::path& output) const;
    void save_depth(const fs::path& output) const;
    void draw(const fs::path& output) const;
};
} // namespace Raytracer