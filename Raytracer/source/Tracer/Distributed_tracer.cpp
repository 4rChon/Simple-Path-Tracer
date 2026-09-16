#include <array>
#include <cstdlib>
#include <vector>
#include "Ad_hoc_material.h"
#include "Distributed_tracer.h"
#include "glm/vec3.hpp"
#include "Ray.h"
#include "Sampler.h"
#include "Scene.h"
#include "shader.h"
#include "spectrum.h"
#include "Tracer.h"
#include "util.h"

namespace Raytracer {
DistributedTracer::DistributedTracer(const std::array<unsigned int, 2> dimensions,
                                     const unsigned int depth)
    : Tracer(dimensions, depth)
{
}

DistributedTracer::~DistributedTracer() {}

glm::vec3 DistributedTracer::trace(Scene& scene, Ray& ray, Sampler& sampler,
                                   const unsigned int depth)
{
    Ray r;
    Intersection i;
    AdHocMaterial* material;
    float pdf;
    glm::vec3 L_d(0.f), L_r(0.f), L_t(0.f), brdf, wi, wo;

    // Compute ray-scene intersection
    if (scene.find_intersection(ray, i)) {
        // If we hit a light source, return its emittance
        if (i.has_light) {
            return i.L_e;
        }

        // Compute direct lighting (N=g_drt_samples_shadow);
        L_d +=
            scene.compute_direct(Shader::stochastic_shader, sampler, i, shadow_samples_);

        // Compute indirect lighting
        if (depth > 0) {
            material = (AdHocMaterial*)i.material;
            wo = -ray.direction_;

            float fresnel = 1.f;
            float eta = material->get_eta();
            float normal_dot_ray = glm::dot(i.geometry_ONB.W, ray.direction_);
            if (eta > 0.f) {
                fresnel = Util::fresnel_schlick(normal_dot_ray, eta, 1.f);
            }
            // Sample reflectance function (N = g_drt_samples_reflection)
            for (auto samples = reflection_samples_; samples > 0; --samples) {
                std::vector<float> xi = sampler.next_uniform_real(2);
                // Get a new direction from reflectance function R
                brdf = material->sample_specular(xi, i.geometry_ONB.W, wo, wi, pdf);
                // numerical stability check + prune redundant bounces
                if (pdf >= clip_min_epsilon && !Spectrum::is_black(brdf)) {
                    r.set(i.P + wi * clip_min_epsilon, wi);
                    float cos_theta = abs(glm::dot(wi, i.shading_ONB.W));
                    L_r +=
                        fresnel * cos_theta * brdf * trace(scene, r, sampler, depth - 1);
                }
            }
            L_r /= reflection_samples_;
        }
    }
    return L_d + L_r;
}
} // namespace Raytracer