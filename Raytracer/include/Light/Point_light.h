#pragma once

#include <glm/vec3.hpp>
#include "ILight.h"

namespace Raytracer {
class PointLight : public ILight {
  private:
    glm::vec3 position_;

  public:
    PointLight(const glm::vec3& position, const glm::vec3& L_e);
    ~PointLight();

    glm::vec3 sample_L(const glm::vec3& P, Light_sample& light_sample) override;
    glm::vec3 sample_L(const std::vector<float>& xi, const glm::vec3& P,
                       Light_sample& light_sample) override;
    bool is_delta() const override
    {
        return true;
    }
};
} // namespace Raytracer