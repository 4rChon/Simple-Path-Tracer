#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include "ILight.h"
#include "IShape.h"

namespace Raytracer {
class AreaLight : public ILight {
  private:
    IShape* shape_;

  public:
    AreaLight(IShape& shape, const glm::vec3& L_e);
    ~AreaLight();

    glm::vec3 sample_L(const glm::vec3& P, Light_sample& light_sample) override;
    glm::vec3 sample_L(const std::vector<float>& xi, const glm::vec3& P,
                       Light_sample& light_sample) override;

    bool is_delta() const override
    {
        return false;
    }
};
} // namespace Raytracer