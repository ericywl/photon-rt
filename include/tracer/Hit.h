#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "Ray.h"
#include <float.h>

class Material;

class Hit
{
public:
    // constructors
    Hit()
    {
        material = NULL;
        t = FLT_MAX;
        hasTex = false;
    }

    Hit(float _t, Material *m, const Vector3f &n)
    {
        t = _t;
        material = m;
        normal = n;
        hasTex = false;
    }

    Hit(float _t, Material *m, const Vector3f &n, const Vector3f &tang)
    {
        t = _t;
        material = m;
        normal = n;
        tangent = tang;
        bitangent = Vector3f::cross(normal, tangent);
        hasTex = false;
    }

    Hit(const Hit &h)
    {
        t = h.t;
        material = h.material;
        normal = h.normal;
        tangent = h.tangent;
        bitangent = h.bitangent;
        hasTex = h.hasTex;
    }

    // destructor
    ~Hit()
    {
    }

    float getT() const
    {
        return t;
    }

    Material *getMaterial() const
    {
        return material;
    }

    const Vector3f &getNormal() const
    {
        return normal;
    }

    const Vector3f &getTangent() const
    {
        return tangent;
    }

    const Vector3f &getBitangent() const
    {
        return bitangent;
    }

    void set(float _t, Material *m, const Vector3f &n)
    {
        t = _t;
        material = m;
        normal = n;
    }

    void set(float _t, Material *m, const Vector3f &n, const Vector3f &tang)
    {
        t = _t;
        material = m;
        normal = n;
        tangent = tang;
        bitangent = Vector3f::cross(normal, tangent);
    }

    void setTexCoord(const Vector2f &coord)
    {
        texCoord = coord;
        hasTex = true;
    }

    bool hasTex;
    Vector2f texCoord;

private:
    float t;
    Material *material;
    Vector3f normal;
    Vector3f tangent;
    Vector3f bitangent;
};

inline ostream &operator<<(ostream &os, const Hit &h)
{
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
