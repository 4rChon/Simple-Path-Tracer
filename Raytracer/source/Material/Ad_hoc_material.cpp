#include <cstdlib>
#include <glm/gtx/norm.hpp>
#include <glm/vec3.hpp>
#include <numbers>
#include <vector>
#include "Ad_hoc_material.h"
#include "BxDF.h"
#include "util.h"

namespace Raytracer {

Ad_hoc_material::Ad_hoc_material(const unsigned int type)
    : BxDF(type), k_D(0), k_S(0), k_T(0)
{
}

Ad_hoc_material::Ad_hoc_material(const unsigned int type, const glm::vec3& diffuse,
                                 const glm::vec3& specular, const glm::vec3& transmissive)
    : BxDF(type), k_D(diffuse), k_S(specular), k_T(transmissive)
{
}

Ad_hoc_material::Ad_hoc_material(const unsigned int type, const glm::vec3& diffuse,
                                 const glm::vec3& specular, const glm::vec3& transmissive,
                                 const float roughness)
    : BxDF(type), k_D(diffuse), k_S(specular), k_T(transmissive), beta(roughness)
{
}

Ad_hoc_material::Ad_hoc_material(const unsigned int type, const glm::vec3& diffuse,
                                 const glm::vec3& specular, const glm::vec3& transmissive,
                                 const float roughness, const float refractive_index)
    : BxDF(type), k_D(diffuse), k_S(specular), k_T(transmissive), beta(roughness),
      eta(refractive_index)
{
}

glm::vec3 Ad_hoc_material::diffuse_brdf(const glm::vec3&, const glm::vec3&,
                                        const glm::vec3&) const
{
    return has_type(BxDF::Diffuse) ? k_D / std::numbers::pi_v<float> : glm::vec3(0);
}

glm::vec3 Ad_hoc_material::specular_brdf(const glm::vec3&, const glm::vec3&,
                                         const glm::vec3&) const
{
    return has_type(BxDF::Reflect) ? k_S : glm::vec3(0);
}

glm::vec3 Ad_hoc_material::transmissive_brdf(const glm::vec3&, const glm::vec3&,
                                             const glm::vec3&) const
{
    return has_type(BxDF::Transmit) ? k_T : glm::vec3(0);
}

glm::vec3 Ad_hoc_material::sample_diffuse(const std::vector<float>& xi,
                                          const glm::vec3& x, const glm::vec3& wo,
                                          glm::vec3& wi_out, float& pdf_out) const
{
    wi_out = Util::sample_cosine_weighted_hemisphere(xi[0], xi[1]);

    if (wo.z < 0.f) {
        wi_out.z *= -1.f;
    }

    pdf_out = std::abs(glm::dot(x, wi_out)) / std::numbers::pi_v<float>;
    // conditional pdf of a cosine weighted hemisphere std::abs(glm::dot(x, wo))

    return diffuse_brdf(x, wo, wi_out);
}

// Change to sample reflective
glm::vec3 Ad_hoc_material::sample_specular(const std::vector<float>& xi,
                                           const glm::vec3& x, const glm::vec3& wo,
                                           glm::vec3& wi_out, float& pdf_out) const
{
    wi_out = -glm::reflect(wo, x);

    if (beta != 0) {
        wi_out += beta * Util::sample_sphere(xi[0], xi[1]);
    }

    // auto cos_n = std::pow(abs(glm::dot(glm::reflect(wo, x), wi_out)), beta);
    pdf_out = std::abs(glm::dot(x, wi_out));
    wi_out = glm::normalize(wi_out);

    return specular_brdf(x, wo, wi_out); // *cos_n * abs(glm::dot(x, wi_out));
}

glm::vec3 Ad_hoc_material::sample_transmissive(const std::vector<float>& xi,
                                               const glm::vec3& x, const glm::vec3& wo,
                                               const float eta_sample, glm::vec3& wi_out,
                                               float& pdf_out) const
{
    wi_out = -glm::refract(wo, x, eta_sample);
    if (beta != 0) {
        wi_out += beta * Util::sample_sphere(xi[0], xi[1]);
    }

    pdf_out = std::abs(glm::dot(x, wi_out));
    wi_out = glm::normalize(wi_out);

    return transmissive_brdf(x, wo, wi_out) * eta_sample;
}
} // namespace Raytracer