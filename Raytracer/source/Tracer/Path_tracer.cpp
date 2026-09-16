#include <algorithm>
#include <array>
#include <cstdlib>
#include <vector>
#include "BxDF.h"
#include "glm/vec3.hpp"
#include "IMaterial.h"
#include "Intersection.h"
#include "Path_tracer.h"
#include "Ray.h"
#include "Sampler.h"
#include "Scene.h"
#include "shader.h"
#include "Spectrum.h"
#include "Tracer.h"
#include "util.h"

namespace Raytracer {
Path_tracer::Path_tracer(const std::array<unsigned int, 2> dimensions,
                         const unsigned int depth)
    : Tracer(dimensions, depth)
{
}

Path_tracer::~Path_tracer() {}

glm::vec3 Path_tracer::trace(Scene& scene, Ray& ray, Sampler& sampler,
                             const unsigned int depth)
{
    Intersection i;
    IMaterial* material;
    float pdf;
    glm::vec3 L(0.f), throughput(1.f), brdf, surface_wi, surface_wo, wi, wo;
    bool specular_hit = false;

    for (unsigned int bounces = 0; bounces < depth; ++bounces) {
        // Get next closest intersection for ray r
        if (scene.find_intersection(ray, i)) {
            // Evaluate emissive/direct components
            material = i.material;

            // Evaluate L_e(x, w) term if we hit a light source
            if (i.has_light) {
                // Add contribution only if coming from a specular hit
                // or this is the first bounce
                if (bounces == 0 || specular_hit) {
                    L += i.L_e * throughput;
                }
                break;
            }

            // Register type of material hit and evaluate direct
            // lighting for non-specular materials
            specular_hit = material->has_type(BxDF::Specular);
            if (!specular_hit) {
                L += scene.compute_direct_stochastic(Shader::stochastic_shader, sampler,
                                                     i) *
                     throughput;
            }

            // sampling of brdf requires a direction; also, brdf assumes all
            // directions are outgoing (vectors look away from the surface)
            wo = -ray.direction_;

            // Another assumption made by the BRDF is that vectors are
            // in surface space, where surface normal = [0 0 1]^T
            // convert wo -> surface_wo
            surface_wo = Util::world_to_surface(i.shading_ONB, wo);

            float fresnel = 1.f;
            float eta = material->get_eta();
            glm::vec3 normal(0, 0, 1);
            glm::vec3 transmit_normal(0, 0, 1);
            if (eta > 0.f) {
                float normal_dot_incident = glm::dot(i.shading_ONB.W, ray.direction_);
                if (normal_dot_incident < 0.f) {
                    fresnel = Util::fresnel_schlick(normal_dot_incident, eta, 1.f);
                    eta = 1.f / eta;
                    transmit_normal *= -1.f;
                }
                else {
                    fresnel = Util::fresnel_schlick(normal_dot_incident, 1.f, eta);
                }
            }

            std::vector<float> xi = sampler.next_uniform_real(2);
            if (material->has_type(BxDF::Diffuse)) {
                brdf = material->sample_diffuse(xi, normal, surface_wo, surface_wi, pdf);
            }
            else if (sampler.next_uniform_real(1)[0] <= fresnel) {
                brdf = material->sample_specular(xi, normal, surface_wo, surface_wi, pdf);
            }
            else {
                brdf = material->sample_transmissive(xi, transmit_normal, surface_wo, eta,
                                                     surface_wi, pdf);
            }

            if (pdf <= clip_min_epsilon || Spectrum::is_black(brdf)) {
                break;
            }

            // Convert sampled direction to world space
            wi = Util::surface_to_world(i.shading_ONB, surface_wi);

            // Set ray to next path segment
            ray.set(i.P + wi * clip_min_epsilon, wi);

            // Compute path throughput (contribution)
            // (f(x, wi, wo) * cos(theta)) / pdf(wi)
            throughput *= (brdf * abs(glm::dot(wi, i.shading_ONB.W))) / pdf;

            // Russian roulette to terminate path
            float max_component = Spectrum::max_component(throughput);

            // Attempt RR conditionally
            if ((max_component < 0.3f && bounces > 3) || max_component == 0.f) {
                float q = std::max(0.05f, 1.f - max_component);
                if (sampler.next_uniform_real(1)[0] < q) {
                    break;
                }
                throughput /= 1.f - q;
            }
        }
    }

    // in the end, return accumulated radiance
    return L;
}
} // namespace Raytracer