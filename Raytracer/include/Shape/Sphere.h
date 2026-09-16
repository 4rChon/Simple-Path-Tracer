#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include "IShape.h"

namespace Raytracer
{
  class Sphere
    : public IShape
  {
  private:
    const float r_;
    const float r2_;
    glm::vec3 C_;
  public:
    Sphere(const float r, const glm::vec3& C);
    ~Sphere() override;

    bool intersects(const Ray& ray, Intersection& i_out) override;
    bool intersects(const Ray& ray) override;
    inline float get_surface_area() const override 
    { 
      return 4.f * (float)M_PI * r2_; 
    }
    
    inline glm::vec3 surface_normal(const glm::vec3& point) const
    {
      return glm::normalize(point - C_);
    }

    inline float get_projected_area() const override
    {
      return (float)M_PI * r2_;
    }
    glm::vec3 sample_surface(const std::vector<float>& xi) const;
    glm::vec3 sample_facing_surface(const float x0, const float x1, const glm::vec3& N) const;
    glm::vec3 sample_cosine_weighted_facing_surface(const float x0, const float x1, const glm::vec3& N) const;
  };
}