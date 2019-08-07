#include "Triangle.h"

Triangle::Triangle(const Vector3f &a, const Vector3f &b,
                   const Vector3f &c, Material *m) : Object3D(m)
{
    this->a = a;
    this->b = b;
    this->c = c;

    hasTex = false;
}

bool Triangle::intersect(const Ray &ray, Hit &hit, float tmin)
{
    Vector3f abDif = a - b;
    Vector3f acDif = a - c;
    Vector3f aoDif = a - ray.getOrigin();

    // Calculate beta, gamma and t using barycentric coordinate
    float AD = Matrix3f{abDif, acDif, ray.getDirection()}.determinant();
    float beta = Matrix3f{aoDif, acDif, ray.getDirection()}.determinant() / AD;
    float gamma = Matrix3f{abDif, aoDif, ray.getDirection()}.determinant() / AD;
    float t = Matrix3f{abDif, acDif, a - ray.getOrigin()}.determinant() / AD;

    // Point not in triangle
    if (beta < 0 || gamma < 0 || beta + gamma > 1)
        return false;

    if (t < tmin || t > hit.getT())
        return false;

    vector<float> baryCoords{(1.0f - beta - gamma), beta, gamma};
    Vector3f normal = Vector3f::ZERO;
    Vector2f tex = Vector2f::ZERO;

    // Barycentric interpolation to get normal and UV coordinates
    for (unsigned int i = 0; i < 3; i++)
    {
        normal += baryCoords[i] * normals[i];
        if (hasTex)
            tex += baryCoords[i] * texCoords[i];
    }

    Vector3f edge1 = -abDif;
    Vector3f edge2 = -acDif;
    Vector2f deltaUV1 = texCoords[2] - texCoords[1];
    Vector2f deltaUV2 = texCoords[3] - texCoords[1];

    float numer = (deltaUV1.x() * deltaUV2.y() - deltaUV2.x() * deltaUV1.y());
    Vector3f tangent{
        deltaUV2.y() * edge1.x() - deltaUV1.y() * edge2.x(),
        deltaUV2.y() * edge1.y() - deltaUV1.y() * edge2.y(),
        deltaUV2.y() * edge1.z() - deltaUV1.y() * edge2.z(),
    };
    
    hit.set(t, material, normal.normalized(), tangent.normalized());
    if (hasTex)
        hit.setTexCoord(tex);
    return true;
}