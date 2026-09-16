#include <glm/gtx/norm.hpp>
#include <glm/vec3.hpp>
#include "ILight.h"
#include "Intersection.h"
#include "IPrimitive.h"
#include "Ray.h"
#include "Scene.h"

namespace Raytracer {
Scene::Scene() {}
Scene::~Scene() {}

void Scene::add_primitive(IPrimitive& primitive)
{
    primitive_list_.push_back(&primitive);
}

void Scene::add_light_source(ILight& lightSource)
{
    lightsource_list_.push_back(&lightSource);
}

bool Scene::is_occluded(const glm::vec3& v1, const glm::vec3& v2, const float epsilon) const
{
    glm::vec3 segment = v2 - v1;

    float clipping_range[2] = {epsilon, glm::length(segment) - epsilon};
    Ray r(v1, glm::normalize(segment), clipping_range);

    for (auto& primitive : primitive_list_) {
        if (primitive->intersects(r)) {
            return true;
        }
    }
    return false;
}

bool Scene::find_intersection(const Ray& ray, Intersection& i_out)
{
    Intersection t;

    // Invalidate current intersection
    i_out.is_valid = false;

    // Test all scene objects
    for (auto& primitive : primitive_list_) {
        // Invalidate temporary intersection
        t.is_valid = false;

        // Try ray intersection with shape
        primitive->intersects(ray, t);

        // If closer than our current point, copy
        if (!i_out.is_valid || t.distance < i_out.distance) {
            i_out = t;
        }
    }
    return i_out.is_valid;
}
} // namespace Raytracer