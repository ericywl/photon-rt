#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <vecmath.h>

#include "Ray.h"
#include "Hit.h"
#include "Material.h"
#include "SceneParser.h"
#include "ArgParser.h"

class RayTracer
{
public:
    RayTracer(SceneParser *scene, Arguments *args);

    void setScene(SceneParser *newScene);
    void setBounces(int newBounces);

    Vector3f computeColor(Vector2f &pixel);
    Vector3f computeColor(Vector2f &pixel, Vector3f &normalViz);

private:
    Vector3f traceRay(Ray &ray, Hit &hit, float tMin,
                      unsigned int bounces, float refrIndex,
                      Vector3f &normalViz);
    Vector3f traceRay(Ray &ray, Hit &hit, float tMin,
                      unsigned int bounces, float refrIndex);

    Vector3f computeIllumination(Ray &ray, Hit &hit, Vector3f &point,
                                 Vector3f &normalViz);
    Vector3f computeReflection(Ray &ray, Hit &hit,
                               Vector3f &point, unsigned int bounces,
                               float refrIndex, Vector3f &normal);
    Vector3f computeRefraction(Ray &ray, Hit &hit,
                               Vector3f &point, unsigned int bounces,
                               float refrIndex, float nextRefrIndex,
                               Vector3f &normal, Vector3f &transmittedDir);

protected:
    SceneParser *scene;
    int maxBounces;
    float gamma;
    bool castShadows;
    bool blinn;
};

#endif