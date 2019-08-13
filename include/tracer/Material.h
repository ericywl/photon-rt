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

    ShadeArgs() {}

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
                          const ShadeArgs &sArgs,
                          const bool disregardL = false);

    Vector3f specularShade(const Ray &ray,
                           const Vector3f &norm,
                           const ShadeArgs &sArgs,
                           const bool disregardL = false);

    Vector3f shade(const Ray &ray,
                   const Hit &hit,
                   const ShadeArgs &sArgs,
                   Vector3f &normalViz,
                   const bool noSpecShade = false);

    Vector3f shade(const Ray &ray,
                   const Hit &hit,
                   const ShadeArgs &sArgs,
                   const bool noSpecShade = false);

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

    Vector3f getTexel(Vector2f texCoord)
    {
        if (!t.valid())
        {
            return diffuseColor;
        }

        // Use textures if there is a valid one
        Vector3f texel = t(texCoord[0], texCoord[1]);

        if (c.valid())
        {
            texel = texel + c(texCoord[0], texCoord[1]);
            // Clamp diffuse color to range [0, 1]
            for (unsigned int i = 0; i < 3; i++)
            {
                texel[i] = (texel[i] > 1.0f) ? 1.0f : texel[i];
            }
        }

        return texel;
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
