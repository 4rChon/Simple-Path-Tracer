#include <cmath>
#include <glm/gtx/norm.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include "Coordinate_frame.h"
#include "Intersection.h"
#include "Ray.h"
#include "Sphere.h"
#include "util.h"

namespace Raytracer {
Sphere::Sphere(const float r, const glm::vec3& C) : r_(r), r2_(r * r), C_(C) {}

Sphere::~Sphere() {}

bool Sphere::intersects(const Ray& ray, Intersection& i_out)
{
    glm::vec3 PC = ray.origin_ - C_;
    float b = 2 * glm::dot(ray.direction_, PC);
    float c = glm::length2(PC) - r2_;
    float d = b * b - 4 * c; // Discriminant
    if (d > 0)               // Only interested in two point intersections
    {
        d = sqrt(d);
        // Try with negative discriminant first
        float t = (-b - d) * 0.5f;
        // If t is outside ray segment, try positive discriminant
        if (t <= ray.clip_min) {
            t = (-b + d) * 0.5f;
        }
        // If t is within ray segment bounds, then we have an intersection
        if (t > ray.clip_min && t < ray.clip_max) {
            i_out.shape = this;                         // Store this shape
            i_out.distance = t;                         // Distance from O to P
            i_out.D = ray.direction_;                   // Direction D;
            i_out.P = ray.origin_ + t * ray.direction_; // Point P
            i_out.geometry_ONB.make(
                glm::normalize(i_out.P - C_));      // Surface orthonormal basis
            i_out.shading_ONB = i_out.geometry_ONB; // Shading ONB
            return i_out.is_valid = true;           // Intersection is valid
        }
    }

    return i_out.is_valid = false;
}

bool Sphere::intersects(const Ray& ray)
{
    glm::vec3 PC = ray.origin_ - C_;
    float b = 2 * glm::dot(ray.direction_, PC);
    float c = glm::length2(PC) - r2_;
    float d = b * b - 4 * c;
    if (d > 0) {
        d = sqrt(d);
        float t = (-b - d) * 0.5f;
        if (t <= ray.clip_min) {
            t = (-b + d) * 0.5f;
        }
        if (t > ray.clip_min && t < ray.clip_max) {
            return true;
        }
    }

    return false;
}

glm::vec3 Sphere::sample_surface(const std::vector<float>& xi) const
{
    return (Util::sample_sphere(xi[0], xi[1]) * r_) + C_;
}

glm::vec3 Sphere::sample_facing_surface(const float x0, const float x1,
                                        const glm::vec3& N) const
{
    glm::vec3 surface_point = Util::sample_hemisphere(x0, x1);
    CoordinateFrame ONB(N);
    surface_point = (ONB.get_matrix() * surface_point * r_) + C_;

    return surface_point;
}

glm::vec3 Sphere::sample_cosine_weighted_facing_surface(const float x0, const float x1,
                                                        const glm::vec3& N) const
{
    glm::vec3 surface_point = Util::sample_cosine_weighted_hemisphere(x0, x1);
    CoordinateFrame ONB(N);
    surface_point = (ONB.get_matrix() * surface_point * r_) + C_;

    return surface_point;
}
} // namespace Raytracer