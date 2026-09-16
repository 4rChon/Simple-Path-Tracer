#pragma once

#include <glm/vec3.hpp>
#include <vector>

namespace Raytracer {
struct Light_sample {
    glm::vec3 position;
    glm::vec3 direction;
    float distance;
    float PDF;
};

class ILight {
  protected:
    glm::vec3 L_e_;

  public:
    ILight(const glm::vec3& L_e);
    virtual ~ILight() = 0;

    virtual glm::vec3 sample_L(const glm::vec3& P, Light_sample& light_sample) = 0;
    virtual glm::vec3 sample_L(const std::vector<float>& xi, const glm::vec3& P,
                               Light_sample& light_sample) = 0;
    virtual glm::vec3 L_e()
    {
        return L_e_;
    }
    virtual bool is_delta() const = 0;
};
} // namespace Raytracer