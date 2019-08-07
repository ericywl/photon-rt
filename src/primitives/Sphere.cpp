#include "Sphere.h"

Sphere::Sphere()
{
    center = Vector3f{0};
    radius = 1.0f;
}

Sphere::Sphere(Vector3f center, float radius, Material *material) : Object3D(material)
{
    this->center = center;
    this->radius = radius;
}

bool Sphere::intersect(const Ray &r, Hit &h, float tmin)
{
    // Form ray-sphere intersection quadratic equation
    // Thanks to http://viclw17.github.io/2018/07/16/raytracing-ray-sphere-intersection/
    Vector3f oc = r.getOrigin() - center;

    float a = r.getDirection().absSquared();
    float b = 2 * Vector3f::dot(oc, r.getDirection());
    float c = oc.absSquared() - radius * radius;

    // Compute discriminant squared
    float discriminant = b * b - (4.0f * a * c);

    // If discriminant squared is negative,
    // there is no intersection
    if (discriminant < 0)
        return false;

    // Check both positive and negative solutions just in case
    for (int i = -1; i < 2; i += 2)
    {
        float t = (-b + i * sqrt(discriminant)) / (2.0f * a);
        // If t is not out of bounds and is smaller than current t,
        // set hit with new t etc.
        if (t >= tmin && t <= h.getT())
        {
            Vector3f point = r.pointAtParameter(t);
            float theta = atan2f(-(point.z() - center.z()), point.x() - center.x());
            float phi = acos(-(point.y() - center.y()) / radius);

            // Compute normal and tangent
            Vector3f normal = (point - center).normalized();
            Vector3f tangent = Vector3f{-sin(theta), 0, -cos(theta)};
            h.set(t, material, normal, tangent);

            // Compute texture coordinates
            float u = theta / (2.0f * (float)M_PI) + 0.5f;
            float v = phi / (float)M_PI;
            h.setTexCoord(Vector2f{u, v});

            return true;
        }
    }

    return false;
}
