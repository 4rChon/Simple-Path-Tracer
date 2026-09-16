#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <gflags/gflags.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <Intersection.h>
#include <iostream>
#include <stb_image_write.h>
#include <stdexcept>
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

namespace fs = std::filesystem;

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

void Tracer::save(const std::filesystem::path& output) const
{
    std::ofstream file(output, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Failed to open output file: " + output.string());
    }
    file << "P6\n"
         << width_ << height_ << "\n"
         << "255\n";

    for (const auto& pixel : framebuffer_) {
        const glm::vec3 colour = pixel.colour;

        const std::array<unsigned char, 3> ppm_colour{
            static_cast<unsigned char>(colour.r), static_cast<unsigned char>(colour.g),
            static_cast<unsigned char>(colour.b)};

        file.write(reinterpret_cast<const char*>(ppm_colour.data()),
                   static_cast<std::streamsize>(ppm_colour.size()));
    }

    if (!file) {
        throw std::runtime_error("Failed while writing output file: " + output.string());
    }
}

void Tracer::save_depth(const std::filesystem::path& output) const
{
    std::ofstream file(output, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Failed to open output file: " + output.string());
    }

    file << "P6\n"
         << width_ << height_ << "\n"
         << "255\n";

    for (const auto& pixel : framebuffer_) {
        std::array<unsigned char, 3> ppm_colour{static_cast<unsigned char>(pixel.depth),
                                                static_cast<unsigned char>(pixel.depth),
                                                static_cast<unsigned char>(pixel.depth)};

        file.write(reinterpret_cast<const char*>(ppm_colour.data()),
                   static_cast<std::streamsize>(ppm_colour.size()));
    }

    if (!file) {
        throw std::runtime_error("Failed while writing output file: " + output.string());
    }
}

void Tracer::draw(const std::filesystem::path& output) const
{
    std::vector<unsigned char> image;
    image.reserve(static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_) *
                  3);

    for (const auto& pixel : framebuffer_) {
        const glm::vec3 colour = pixel.colour;

        image.push_back(static_cast<unsigned char>(colour.r));
        image.push_back(static_cast<unsigned char>(colour.g));
        image.push_back(static_cast<unsigned char>(colour.b));
    }

    if (stbi_write_jpg(output.string().c_str(), width_, height_, 3, image.data(), 90) ==
        0) {
        throw std::runtime_error("Failed to write JPEG: " + output.string());
    }
}
} // namespace Raytracer