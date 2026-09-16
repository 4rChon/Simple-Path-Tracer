#include <array>
#include <glm/vec3.hpp>
#include "Ad_hoc_material.h"
#include "BxDF.h"
#include "Intersection.h"
#include "Ray.h"
#include "Sampler.h"
#include "Scene.h"
#include "shader.h"
#include "Tracer.h"
#include "util.h"
#include "Whitted_tracer.h"

namespace Raytracer {
Whitted_tracer::Whitted_tracer(const std::array<unsigned int, 2> dimensions,
                               const unsigned int depth)
    : Tracer(dimensions, depth)
{
}

Whitted_tracer::~Whitted_tracer() {}

glm::vec3 Whitted_tracer::trace(Scene& scene, Ray& ray, Sampler& sampler,
                                const unsigned int depth)
{
    Ray r;
    Intersection i;
    glm::vec3 i_D(0), i_S(0), i_T(0);
    AdHocMaterial* material;

    // Find closest point of intersection
    if (scene.find_intersection(ray, i)) {
        // Compute direct lighting (diffuse)
        i_D = scene.compute_direct(Shader::whitted_shader, sampler, i);
        if (depth > 0) {
            // Get material at point of intersection
            material = (AdHocMaterial*)i.material;
            if (material->has_type(BxDF::Specular)) {
                float eta = material->get_eta();
                float fresnel = 1.F;
                float normal_dot_incident = glm::dot(i.shading_ONB.W, ray.direction_);
                if (eta > 0.F) {
                    if (normal_dot_incident < 0.F) {
                        eta = 1.F / eta;
                        fresnel = Util::fresnel_schlick(normal_dot_incident, eta, 1.F);
                    }
                    else {
                        fresnel = Util::fresnel_schlick(normal_dot_incident, 1.F, eta);
                    }
                }
                // Specular reflection
                if (material->has_type(BxDF::Reflect)) {
                    r.set(i.P, glm::reflect(ray.direction_, i.shading_ONB.W));
                    i_S = fresnel * trace(scene, r, sampler, depth - 1) * material->k_S;
                }

                // Specular transmission
                if (material->has_type(BxDF::Transmit)) {
                    // Is entering or exiting material?
                    if (normal_dot_incident < 0.F) {
                        r.set(i.P, glm::normalize(glm::refract(ray.direction_,
                                                               i.shading_ONB.W, eta)));
                    }
                    else {
                        r.set(i.P, glm::normalize(glm::refract(ray.direction_,
                                                               -i.shading_ONB.W, eta)));
                    }

                    i_T = (1 - fresnel) * trace(scene, r, sampler, depth - 1) *
                          material->k_T;
                }
            }
        }
    }

    return i_D + i_S + i_T;
}
} // namespace Raytracer
