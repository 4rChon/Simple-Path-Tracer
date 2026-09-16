#pragma once

#include <glm/vec3.hpp>
#include <vector>

namespace Raytracer {
class IMaterial {
  public:
    IMaterial();
    virtual ~IMaterial() = 0;

    virtual float get_eta() const
    {
        return 0.f;
    }
    virtual glm::vec3 diffuse_brdf(const glm::vec3& x, const glm::vec3& wo,
                                   const glm::vec3& wi) const = 0;

    virtual glm::vec3 specular_brdf(const glm::vec3& x, const glm::vec3& wo,
                                    const glm::vec3& wi) const = 0;

    virtual glm::vec3 transmissive_brdf(const glm::vec3& x, const glm::vec3& wo,
                                        const glm::vec3& wi) const = 0;

    virtual glm::vec3 sample_diffuse(const std::vector<float>& xi, const glm::vec3& x,
                                     const glm::vec3& wo, glm::vec3& wi_out,
                                     float& pdf_out) const = 0;

    virtual glm::vec3 sample_specular(const std::vector<float>& xi, const glm::vec3& x,
                                      const glm::vec3& wo, glm::vec3& wi_out,
                                      float& pdf_out) const = 0;

    virtual glm::vec3 sample_transmissive(const std::vector<float>& xi,
                                          const glm::vec3& x, const glm::vec3& wo,
                                          const float eta, glm::vec3& wi_out,
                                          float& pdf_out) const = 0;

    virtual bool has_type(const int type) const = 0;
};
} // namespace Raytracer