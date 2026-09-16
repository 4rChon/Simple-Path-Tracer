#include <algorithm>
#include <cmath>
#include <glm/gtx/norm.hpp>
#include <glm/vec3.hpp>
#include <numbers>
#include <string>
#include <string_view>
#include <vector>

#include "BxDF.h"
#include "Coordinate_frame.h"
#include "Pixel.h"
#include "util.h"

namespace Raytracer::Util {
std::vector<std::string> split(std::string_view input, std::string_view delimiters)
{
    std::vector<std::string> result;
    std::size_t start = input.find_first_not_of(delimiters);

    while (start != std::string_view::npos) {
        const std::size_t end = input.find_first_of(delimiters, start);
        result.emplace_back(input.substr(start, end - start));
        if (end == std::string_view::npos) {
            break;
        }

        start = input.find_first_not_of(delimiters, end);
    }

    return result;
}

BxDF::Type get_material_type(const std::string& type)
{
    if (type == "diffuse") {
        return BxDF::Diffuse;
    }
    if (type == "specular") {
        return BxDF::Specular;
    }
    if (type == "reflection") {
        return Raytracer::BxDF::Reflect;
    }
    if (type == "glossy") {
        return Raytracer::BxDF::Glossy;
    }
    if (type == "fresnel" || type == "dielectric") {
        return BxDF::Transmit;
    }
    return BxDF::Diffuse;
}

glm::vec3 sample_hemisphere(const float x0, const float x1)
{
    double z = 1 - x0;
    double r = sqrt(1 - z * z);
    double phi = 2 * std::numbers::pi_v<float> * x1;
    double x = r * cos(phi);
    double y = r * sin(phi);

    return glm::vec3(x, y, z);
}

glm::vec3 sample_cosine_weighted_hemisphere(const float x0, const float x1)
{
    double r = sqrt(x0);
    double phi = 2 * std::numbers::pi_v<float> * x1;
    double x = r * cos(phi);
    double y = r * sin(phi);
    double z = sqrt(std::max(0.f, 1 - x0));

    return glm::vec3(x, y, z);
}

glm::vec3 sample_sphere(const float x0, const float x1)
{
    double z = 1 - (2 * x0);
    double r = sqrt(1 - z * z);
    double phi = 2 * std::numbers::pi_v<float> * x1;
    double x = r * cos(phi);
    double y = r * sin(phi);

    return glm::vec3(x, y, z);
}

float fresnel_schlick(const float cos_theta, const float eta_0, const float eta_1)
{
    float clamped_cos_theta = glm::abs(cos_theta);
    float R_0 = powf((eta_0 - eta_1) / (eta_0 + eta_1), 2);
    return R_0 + ((1 - R_0) * powf((1 - clamped_cos_theta), 5));
}

float fresnel(const float cos_theta, const float eta_0, const float eta_1)
{
    float cosi = glm::clamp(cos_theta, -1.f, 1.f);
    float etai = eta_0, etat = eta_1;

    float sint = eta_0 / eta_1 * sqrtf(glm::max(0.f, 1 - cosi * cosi));

    if (sint >= 1) {
        return 1.f;
    }
    else {
        float cost = sqrtf(glm::max(0.f, 1 - sint * sint));
        cosi = fabsf(cosi);
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        return (Rs * Rs + Rp * Rp) / 2.f;
    }
}

glm::vec3 world_to_surface(const CoordinateFrame& ONB, const glm::vec3& v)
{
    return glm::inverse(ONB.get_matrix()) * v;
}

glm::vec3 surface_to_world(const CoordinateFrame& ONB, const glm::vec3& v)
{
    return ONB.get_matrix() * v;
}

glm::vec3 max_colour(const std::vector<Pixel>& pixelbuffer)
{
    glm::vec3 max_c = glm::vec3(0);
    for (const auto& pixel : pixelbuffer) {
        glm::vec3 c = pixel.colour;
        if (c.r > max_c.r) {
            max_c.r = c.r;
        }

        if (c.g > max_c.g) {
            max_c.g = c.g;
        }

        if (c.b > max_c.b) {
            max_c.b = c.b;
        }
    }

    return max_c;
}

float max_depth(const std::vector<Pixel>& pixelbuffer)
{
    float max_d = 0.f;
    for (const auto& pixel : pixelbuffer) {
        if (pixel.depth > max_d) {
            max_d = pixel.depth;
        }
    }

    return max_d;
}
} // namespace Raytracer::Util