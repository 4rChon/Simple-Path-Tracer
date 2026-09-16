#pragma once

#include <glm/vec3.hpp>
#include "BxDF.h"

namespace Raytracer {
class Ad_hoc_material : public BxDF {
  public:
    const glm::vec3 k_D;
    const glm::vec3 k_S;
    const glm::vec3 k_T;
    const float beta = 0.f;
    const float eta = 0.f;

    Ad_hoc_material(const unsigned int type);

    Ad_hoc_material(const unsigned int type, const glm::vec3& diffuse,
                    const glm::vec3& specular, const glm::vec3& transmissive);

    Ad_hoc_material(const unsigned int type, const glm::vec3& diffuse,
                    const glm::vec3& specular, const glm::vec3& transmissive,
                    const float roughness);

    Ad_hoc_material(const unsigned int type, const glm::vec3& diffuse,
                    const glm::vec3& specular, const glm::vec3& transmissive,
                    const float roughness, const float refractive_index);

    float get_eta() const override
    {
        return eta;
    }

    glm::vec3 diffuse_brdf(const glm::vec3& x, const glm::vec3& wo,
                           const glm::vec3& wi) const override;

    glm::vec3 specular_brdf(const glm::vec3& x, const glm::vec3& wo,
                            const glm::vec3& wi) const override;

    glm::vec3 transmissive_brdf(const glm::vec3& x, const glm::vec3& wo,
                                const glm::vec3& wi) const override;

    glm::vec3 sample_diffuse(const std::vector<float>& xi, const glm::vec3& x,
                             const glm::vec3& wo, glm::vec3& wi_out,
                             float& pdf_out) const override;

    glm::vec3 sample_specular(const std::vector<float>& xi, const glm::vec3& x,
                              const glm::vec3& wo, glm::vec3& wi_out,
                              float& pdf_out) const override;

    glm::vec3 sample_transmissive(const std::vector<float>& xi, const glm::vec3& x,
                                  const glm::vec3& wo, const float eta, glm::vec3& wi_out,
                                  float& pdf_out) const override;
};

} // namespace Raytracer
