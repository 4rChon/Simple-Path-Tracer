#define _CRT_SECURE_NO_WARNINGS

#include <array>
#include <cstdio>
#include <gflags/gflags.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <Intersection.h>
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include "Camera.h"
#include "Pixel.h"
#include "Ray.h"
#include "Sampler.h"
#include "Scene.h"
#include "Tracer.h"
#include "util.h"

DEFINE_bool(progress, false, "Print progress every 1000 pixels");
DEFINE_bool(stratified_sampling, true, "Use a stratified sampling strategy");
DECLARE_bool(depth_map);

constexpr auto step_interval = 1000;

namespace Raytracer {
Tracer::Tracer(const std::array<unsigned int, 2> dimensions, const unsigned int depth)
    : width_(dimensions[0]), height_(dimensions[1]), depth_(depth)
{
    for (auto y = 0U; y < height_; ++y) {
        for (auto x = 0U; x < width_; ++x) {
            framebuffer_.emplace_back(Pixel(x, y));
        }
    }
}

Tracer::~Tracer() {}

std::vector<Pixel> Tracer::render(Scene& scene, Camera& camera, Sampler& sampler)
{
    unsigned int step = 0;
    auto before = GetTickCount64();
    for (auto& pixel : framebuffer_) {
        auto L = glm::vec3(0); // Initial radiance
        for (auto i = 0U; i < sampler.spp; i++) {
            Raytracer::Sample spp_sample = FLAGS_stratified_sampling
                                               ? sampler.next_stratified_sample()
                                               : sampler.next_uniform_sample();
            float xx = (float)(pixel.x + spp_sample.jitter.x) / (float)width_;
            float yy = (float)(pixel.y + spp_sample.jitter.y) / (float)height_;

            if (camera.get_type() == Camera::Pinhole) {
                Ray ray = camera.get_ray(xx, yy);
                L += trace(scene, ray, sampler, depth_) * sampler.get_weight();
            }
            else if (camera.get_type() == Camera::Lens) {
                Raytracer::Sample dof_sample = sampler.next_uniform_sample();
                Ray ray = camera.get_ray(xx, yy, dof_sample.jitter);
                L += trace(scene, ray, sampler, depth_) * sampler.get_weight();
            }
            else {
                throw "Invalid camera type.";
            }
        };

        pixel.colour = L;

        if (FLAGS_progress && step++ % step_interval == 0) {
            printf("%d/%d\n", step++, height_ * width_);
        }
    }

    printf("elapsed time: %f seconds \n",
           double(GetTickCount64() - before) / step_interval);

    return framebuffer_;
}

void Tracer::render_depth_map(Scene& scene, Camera& camera)
{
    std::cout << "Rendering depthmap\n";

    for (auto& pixel : framebuffer_) {
        if (camera.get_type() == Camera::Pinhole) {
            Ray ray = camera.get_ray(pixel.x / (float)width_, pixel.y / (float)height_);
            Intersection i;

            if (scene.find_intersection(ray, i)) {
                pixel.depth = i.distance;
            }
        }

        if (camera.get_type() == Camera::Lens) {
            Ray ray = camera.get_ray(pixel.x / (float)width_, pixel.y / (float)height_,
                                     glm::vec2(0.f, 0.f));
            Intersection i;

            if (scene.find_intersection(ray, i)) {
                pixel.depth = i.distance;
            }
        }
    }
}

void Tracer::tone_map_linear()
{
    glm::vec3 max_colour = Util::max_colour(framebuffer_);
    for (auto& pixel : framebuffer_) {
        pixel.colour = 255.f * (pixel.colour / max_colour);
    }
}

void Tracer::tone_map_sigmoid()
{
    for (auto& pixel : framebuffer_) {
        pixel.colour = 255.f * (pixel.colour / (1.f + pixel.colour));
    }
}

void Tracer::tone_map_depth()
{
    for (auto& pixel : framebuffer_) {
        pixel.depth = 255.f * (1.f - (pixel.depth / (1.f + pixel.depth)));
    }
}

void Tracer::save(const std::string& path, const std::string& filename) const
{
    std::string output = path + "/" + filename;
    FILE* fp = fopen(output.c_str(), "wb+");
    (void)fprintf(fp, "P6\n%d %d\n255\n", width_, height_);

    for (const auto& pixel : framebuffer_) {
        glm::vec3 colour = pixel.colour;
        static std::array<unsigned char, 3> ppm_colour{
            (unsigned char)colour.r, (unsigned char)colour.g, (unsigned char)colour.b};
        (void)fwrite(ppm_colour.data(), 1, 3, fp);
    }

    (void)fclose(fp);
}

void Tracer::save_depth(const std::string& path, const std::string& filename) const
{
    std::string output = path + "/depth_" + filename;

    FILE* fp = fopen(output.c_str(), "wb+");
    (void)fprintf(fp, "P6\n%d %d\n255\n", width_, height_);

    for (const auto& pixel : framebuffer_) {
        static std::array<unsigned char, 3> ppm_colour{(unsigned char)pixel.depth,
                                                       (unsigned char)pixel.depth,
                                                       (unsigned char)pixel.depth};
        (void)fwrite(ppm_colour.data(), 1, 3, fp);
    }

    (void)fclose(fp);
}

void Tracer::draw() const {}
} // namespace Raytracer