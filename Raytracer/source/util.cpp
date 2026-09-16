#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include "util.h"

#include <math.h>
#include <glm/gtx/norm.hpp>
#include <algorithm>

namespace Raytracer
{
  namespace Util
  {
    void split(const std::string& s, const char* delim, std::vector<std::string>& v)
    {
      auto l_size = s.length();
      char* buffer = (char*)malloc(sizeof(char)*l_size);
      s.copy(buffer, l_size, 0);
      buffer[l_size] = '\0';
      char* split_tokens = strtok(buffer, delim);
      while (split_tokens != NULL)
      {
        v.push_back(split_tokens);
        split_tokens = strtok(NULL, delim);
      }
    }

    BxDF::Type get_material_type(const std::string& type)
    {
      if (type == "diffuse")
        return BxDF::Diffuse;
      if (type == "specular")
        return BxDF::Specular;
      if (type == "reflection")
        return Raytracer::BxDF::Reflect;
      if (type == "glossy")
        return Raytracer::BxDF::Glossy;
      if (type == "fresnel" || type == "dielectric")
        return BxDF::Transmit;
      return BxDF::Diffuse;
    }

    glm::vec3 sample_hemisphere(const float x0, const float x1)
    {
      double z = 1 - x0;
      double r = sqrt(1 - z*z);
      double phi = 2 * M_PI * x1;
      double x = r * cos(phi);
      double y = r * sin(phi);

      return glm::vec3(x, y, z);
    }

    glm::vec3 sample_cosine_weighted_hemisphere(const float x0, const float x1)
    {
      double r = sqrt(x0);
      double phi = 2 * M_PI * x1;
      double x = r * cos(phi);
      double y = r * sin(phi);
      double z = sqrt(std::max(0.f, 1 - x0));

      return glm::vec3(x, y, z);
    }

    glm::vec3 sample_sphere(const float x0, const float x1)
    {
      double z = 1 - (2 * x0);
      double r = sqrt(1 - z*z);
      double phi = 2 * M_PI * x1;
      double x = r * cos(phi);
      double y = r * sin(phi);

      return glm::vec3(x, y, z);
    }

    float fresnel_schlick(const float cos_theta,
                          const float eta_0,
                          const float eta_1)
    {
      float clamped_cos_theta = glm::abs(cos_theta);
      float R_0 = powf((eta_0 - eta_1) / (eta_0 + eta_1), 2);
      return R_0 + ((1 - R_0) * powf((1 - clamped_cos_theta), 5));
    }

    float fresnel(const float cos_theta,
                  const float eta_0,
                  const float eta_1)
    {
      float cosi = glm::clamp(cos_theta, -1.f, 1.f);
      float etai = eta_0, etat = eta_1;

      float sint = eta_0 / eta_1 * sqrtf(glm::max(0.f, 1 - cosi * cosi));

      if (sint >= 1) {
        return 1.f;
      }
      else
      {
        float cost = sqrtf(glm::max(0.f, 1 - sint * sint));
        cosi = fabsf(cosi);
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        return (Rs * Rs + Rp * Rp) / 2.f;
      }
    }

    glm::vec3 world_to_surface(const Coordinate_frame& ONB, const glm::vec3& v)
    {
      return glm::inverse(ONB.get_matrix()) * v;
    }

    glm::vec3 surface_to_world(const Coordinate_frame& ONB, const glm::vec3& v)
    {
      return ONB.get_matrix() * v;
    }

    glm::vec3 max_colour(std::vector<Pixel>& pixelbuffer)
    {
      glm::vec3 max_c = glm::vec3(0);
      for (auto& pixel : pixelbuffer)
      {
        glm::vec3 c = pixel.colour;
        if (c.r > max_c.r)
        {
          max_c.r = c.r;
        }

        if (c.g > max_c.g)
        {
          max_c.g = c.g;
        }

        if (c.b > max_c.b)
        {
          max_c.b = c.b;
        }
      }

      return max_c;
    }

    float max_depth(std::vector<Pixel>& pixelbuffer)
    {
      float max_d = 0.f;
      for (auto& pixel : pixelbuffer)
      {
        if (pixel.depth > max_d)
        {
          max_d = pixel.depth;
        }
      }

      return max_d;
    }
  }
}