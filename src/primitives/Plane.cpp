#include "Plane.h"

float mapToRange(float x, float outMin, float outMax)
{
    float inMin = -1, inMax = 1;
    if (x < -1 || x > 1)
    {
        bool test = (fmod(floor(x), 2) == 0);
        inMin = test ? floorf(x) - 1 : floorf(x);
        inMax = test ? ceilf(x) : ceilf(x) + 1;
    }

    return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

Vector3f computePrimaryBasis(Vector3f &normal)
{
    Vector3f a = Vector3f::UP;
    if (Vector3f::dot(normal, a) == 1.0f)
    {
        a = -Vector3f::FORWARD;
    }

    return Vector3f::cross(normal, a).normalized();
}

Plane::Plane(const Vector3f &normal, float d, Material *m) : Object3D(m)
{
    this->normal = normal.normalized();
    this->d = d;
    this->textureScale = Vector2f{1.0f, 1.0f};
}

Plane::Plane(const Vector3f &normal, float d, Material *m, Vector2f textureScale)
    : Plane(normal, d, m)
{
    this->textureScale = textureScale;
}

bool Plane::intersect(const Ray &r, Hit &h, float tmin)
{
    float n0 = Vector3f::dot(normal, r.getOrigin());
    float nd = Vector3f::dot(normal, r.getDirection());

    // Avoid division-by-zero error ie. parallel
    if (fabs(nd) < 1e-8)
        return false;

    // Plane intersection formula
    float t = (d - n0) / nd;
    if (t < tmin || t > h.getT())
        return false;

    // Compute basis for UV coordinates
    Vector3f uBasis = computePrimaryBasis(normal);
    Vector3f vBasis = Vector3f::cross(normal, uBasis);

    // Set hit with uBasis as tangent
    h.set(t, material, normal, uBasis);

    // Compute UV coordinates
    Vector3f point = r.pointAtParameter(t);
    float u = Vector3f::dot(uBasis, point) / textureScale.x();
    float v = Vector3f::dot(vBasis, point) / textureScale.y();

    // Map coordinates to [0, 1] with repeats
    u = mapToRange(u, 0, 1);
    v = mapToRange(v, 0, 1);
    h.setTexCoord(Vector2f{u, v});

    return true;
}