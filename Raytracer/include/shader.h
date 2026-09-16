#pragma once

#include <cstdlib>
#include <glm/vec3.hpp>
#include "ILight.h"
#include "IMaterial.h"
#include "Intersection.h"
#include "Sampler.h"
#include "Scene.h"
#include "spectrum.h"

/* For point lighting
    I(x -> V_p) = G(P_p -> x) * L(P_p -> x) * R(P_p -> x -> V_p)
    G = Visibility function
    L = Radiance function
    R = Reflectance function
   For area lighting
    I(x -> V_p) = integral_{A_s}(L(y -> x) * R(y -> x -> V_p) * (cos(theta_x) * cos(theta_y))/r^2_{xy})dy
    A_s is the light surface
    y is a point on A_s
    theta_x = dot(x->y, x.normal)
    theta_y = dot(x->y, y.normal)
    r = |x -> y|
*/

namespace Raytracer {
namespace Shader {
static const auto whitted_shader = [](Scene* scene, Sampler&, ILight* light, Intersection& i) {
    Light_sample light_sample;
    // Get light intensity and populate light sample structure (pos, dir, dist)
    glm::vec3 L_e = light->sample_L(i.P, light_sample);
    // Are sampled light position and point of intersection mutually visible?
    if (scene->is_occluded(i.P, light_sample.position)) {
        return glm::vec3(0);
    }
    // Yes, shade point
    IMaterial* material = i.material;
    // Get incoming vector (wi) view/outgoing vector (wo)
    glm::vec3 wi = -light_sample.direction;
    glm::vec3 wo = -i.D;
    // Evaluate reflectivity function wi and wo
    glm::vec3 brdf = material->diffuse_brdf(i.P, wi, wo);
    // If no light can be reflected, return
    if (Spectrum::is_black(brdf)) {
        return glm::vec3(0.f);
    }
    // Le * reflectivity coefficient * foreshortening (Lambert's Law)
    float cos_theta = abs(glm::dot(wi, i.shading_ONB.W));
    return L_e * brdf * cos_theta;
};

static const auto stochastic_shader = [](Scene* scene, Sampler& sampler, ILight* light, Intersection& i) {
    Light_sample light_sample;
    glm::vec3 L_e = light->is_delta() ? light->sample_L(i.P, light_sample)
                                      : light->sample_L(sampler.next_uniform_real(2), i.P, light_sample);

    if (Spectrum::is_black(L_e) || scene->is_occluded(i.P, light_sample.position)) {
        return glm::vec3(0.f);
    }
    else {
        IMaterial* material = i.material;

        glm::vec3 wi = -light_sample.direction;
        glm::vec3 wo = -i.D;

        glm::vec3 brdf = material->diffuse_brdf(i.P, wi, wo);

        if (Spectrum::is_black(brdf)) {
            return glm::vec3(0);
        }

        float cos_theta_x = abs(glm::dot(i.shading_ONB.W, wi));
        return (L_e * brdf * cos_theta_x) / light_sample.PDF;
    }
};
} // namespace Shader
} // namespace Raytracer