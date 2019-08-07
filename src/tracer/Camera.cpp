#include "Camera.h"

PerspectiveCamera::PerspectiveCamera(const Vector3f &center,
                                     const Vector3f &direction,
                                     const Vector3f &up,
                                     float angle)
{
    this->center = center;
    this->direction = direction;
    this->up = up;
    this->horizontal = Vector3f::cross(direction, up);
    this->angle = angle;

    // Calculate u, v, w and D based on formula given in assignment
    w = direction.normalized();
    u = Vector3f::cross(w, up);
    v = Vector3f::cross(u, w);
    D = 1.0f / tan(angle / 2.0f);
}

Ray PerspectiveCamera::generateRay(const Vector2f &pixel)
{
    // Generate ray as per lecture notes
    Vector3f dir = (pixel.x() * u + pixel.y() * v / aspect + D * w).normalized();
    return Ray(center, dir);
}

float PerspectiveCamera::getTMin() const
{
    return 0.0f;
}