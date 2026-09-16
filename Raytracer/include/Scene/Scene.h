#pragma once
#include <vector>

#include <glm/vec3.hpp>

#include "Intersection.h"
#include "IPrimitive.h"
#include "ILight.h"
#include "Ray.h"
#include "Sampler.h"

namespace Raytracer
{
  class Scene
  {
  private:
    std::vector<IPrimitive*> primitive_list_;
    std::vector<ILight*> lightsource_list_;
  public:
    Scene();
    ~Scene();

    void add_primitive(IPrimitive& primitive);
    void add_light_source(ILight& lightsource);

    bool is_occluded(const glm::vec3& x,
                     const glm::vec3& y,
                     const float epsilon = clip_min_epsilon) const;

    bool find_intersection(const Ray& ray, Intersection& i_out);

    template<typename Func>
    glm::vec3 compute_direct(Func&& shader, Sampler& sampler, Intersection& intersection)
    {
      glm::vec3 L = glm::vec3(0.f);
      for (auto& lightsource : lightsource_list_)
      {
        L += shader(this, sampler, lightsource, intersection);
      }

      return L;
    }

    template<typename Func>
    glm::vec3 compute_direct(Func&& shader, Sampler& sampler, Intersection& intersection, const unsigned int N)
    {
      glm::vec3 L = glm::vec3(0.f);
      for (auto& lightsource : lightsource_list_)
      {
        if (lightsource->is_delta())
        {
          /* if point light sample once */
          L += shader(this, sampler, lightsource, intersection);
        }
        else
        {
          /* otherwise sample N times over surface of area light*/
          for (unsigned int i = 0; i < N; ++i)
          {
            L += (shader(this, sampler, lightsource, intersection)) / (float)N;
          }
        }
      }

      return L;
    }

    template<typename Func>
    glm::vec3 compute_direct_stochastic(Func&& shader, Sampler& sampler, Intersection& intersection)
    {
      unsigned int index = sampler.next_uniform_int(1)[0] % lightsource_list_.size();
      return shader(this, sampler, lightsource_list_[index], intersection);
    }
  };
}