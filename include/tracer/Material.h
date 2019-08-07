#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vector>
#include <vecmath.h>

#include "Ray.h"
#include "Hit.h"
#include "Texture.hpp"
#include "NormalMap.hpp"
#include "Noise.h"

// Lighting etc. container is defined here because there will
// be circular dependency when including "Light.h"
struct ShadeArgs
{
    float distToLight;
    Vector3f dirToLight, lightCol;
    bool blinn;
    float gamma;

    ShadeArgs(bool b, float g)
    {
        blinn = b;
        gamma = g;
    }
};

class Material
{
public:
    bool disregardLighting = false;

    Material(const Vector3f &d_color, const Vector3f &s_color, float s,
             float r) : diffuseColor(d_color), refractionIndex(r), shininess(s), specularColor(s_color)
    {
    }

    virtual ~Material()
    {
    }

    float getRefractionIndex()
    {
        return refractionIndex;
    }
    Vector3f getDiffuseColor()
    {
        return diffuseColor;
    }
    Vector3f getSpecularColor()
    {
        return specularColor;
    }

    void setNoise(const Noise &n)
    {
        noise = n;
    }

    Vector3f diffuseShade(const Ray &ray,
                          const Hit &hit,
                          const Vector3f &norm,
                          const ShadeArgs &sArgs);

    Vector3f specularShade(const Ray &ray,
                           const Vector3f &norm,
                           const ShadeArgs &sArgs);

    Vector3f shade(const Ray &ray,
                   const Hit &hit,
                   const ShadeArgs &sArgs,
                   Vector3f &normalViz,
                   bool noSpecShade = false);

    Vector3f shade(const Ray &ray,
                   const Hit &hit,
                   const ShadeArgs &sArgs,
                   bool noSpecShade = false);

    void loadTexture(const char *filename)
    {
        t.load(filename);
    }

    void loadCloud(const char *filename)
    {
        c.load(filename);
    }

    void loadNormalMap(const char *filename)
    {
        nm.load(filename);
    }

protected:
    Vector3f diffuseColor;
    Vector3f specularColor;
    float shininess;
    float refractionIndex;
    Texture t;
    Texture c;
    NormalMap nm;
    Noise noise;
};

#endif // MATERIAL_H
