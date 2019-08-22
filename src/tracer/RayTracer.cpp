#include "RayTracer.h"

#define EPSILON 0.001

bool checkInShadow(Group *g, Vector3f &point, Vector3f &dirToLight, float distToLight)
{
    // Check if pixel is in shadow using shadow ray
    Ray shadowRay{point, dirToLight};
    Hit shadowHit{distToLight, 0, 0};

    g->intersect(shadowRay, shadowHit, EPSILON, true);
    // Pixel in shadow if t is altered
    return shadowHit.getT() != distToLight;
}

RayTracer::RayTracer(SceneParser *scene, Arguments *args, PhotonMap *pMap)
{
    this->scene = scene;
    this->photonMap = pMap;
    maxBounces = args->rayBounces;
    castShadows = args->shadows;
    blinn = args->useBlinn;
    gamma = args->gamma;
    nearestN = args->nearestNeighbors;
    secondaryRays = args->secondaryRays;
}

void RayTracer::setScene(SceneParser *newScene)
{
    this->scene = newScene;
}

void RayTracer::setBounces(int newBounces)
{
    this->maxBounces = newBounces;
}

Vector3f RayTracer::computeColor(Vector2f &pixel, Vector3f &normalViz)
{
    Hit hit;
    Ray ray = scene->getCamera()->generateRay(pixel);
    float tMin = scene->getCamera()->getTMin();
    Vector3f direct = traceRay(ray, hit, tMin, maxBounces, 1.0f, normalViz);
    Vector3f indirect, caustic;
    if (photonMap->ready)
    {
        indirect = 20.0f * computeIndirect(ray, hit, EPSILON);
        caustic = 40.0f * photonMap->knnRadianceEstimate(ray, hit, nearestN, true);
    }

    return direct + indirect + caustic;
}

Vector3f RayTracer::computeColor(Vector2f &pixel)
{
    Vector3f temp;
    return computeColor(pixel, temp);
}

Vector3f RayTracer::computeIndirect(Ray &ray, Hit &hit, float tMin)
{
    Vector3f indirect = Vector3f::ZERO;

    if (secondaryRays == 0) return indirect;

    for (unsigned int i = 0; i < secondaryRays; i++)
    {
        Vector3f dir = randomUnitVector();
        if (Vector3f::dot(dir, hit.getNormal()) < 0.0f)
        {
            dir = -dir;
        }

        Hit iHit;
        Ray iRay{ray.pointAtParameter(hit.getT()), dir};
        if (scene->getGroup()->intersect(iRay, iHit, tMin))
        {
            indirect += photonMap->knnRadianceEstimate(iRay, iHit, nearestN, false);
        }
    }

    return indirect / secondaryRays;
}

Vector3f RayTracer::computeIllumination(Ray &ray, Hit &hit, Vector3f &point,
                                        Vector3f &normalViz)
{
    // For each light, get illumination and shade accordingly (diffuse only)
    Vector3f illumColor = Vector3f::ZERO;
    for (int i = 0; i < scene->getNumLights(); i++)
    {
        ShadeArgs sArgs{blinn, gamma};
        Light *light = scene->getLight(i);
        light->getIllumination(point, sArgs.dirToLight,
                               sArgs.lightCol, sArgs.distToLight);

        // Compute diffuse shading color
        // If maxBounces > 0, we don't want any specular shading because
        // that will be computed via ray tracing
        Vector3f shadeColor = hit.getMaterial()->shade(
            ray, hit, sArgs, normalViz, maxBounces > 0);

        if (!castShadows ||
            (castShadows && !checkInShadow(scene->getGroup(), point,
                                           sArgs.dirToLight, sArgs.distToLight)))
        {
            // Shade the pixel if shadow-casting disabled or pixel not in shadow
            illumColor += shadeColor;
        }
    }

    return illumColor;
}

Vector3f RayTracer::computeReflection(Ray &ray, Hit &hit,
                                      Vector3f &point, unsigned int bounces,
                                      float refrIndex, Vector3f &normal)
{
    // Reflection
    Vector3f reflectedDir = mirrorDirection(normal, ray.getDirection());
    Ray reflectedRay{point, reflectedDir};
    Hit reflectHit;
    Vector3f reflColor = traceRay(reflectedRay, reflectHit, EPSILON,
                                  bounces - 1, refrIndex);
    return reflColor * hit.getMaterial()->getSpecularColor();
}

Vector3f RayTracer::computeRefraction(Ray &ray, Hit &hit,
                                      Vector3f &point, unsigned int bounces,
                                      float refrIndex, float nextRefrIndex,
                                      Vector3f &normal, Vector3f &transmittedDir)
{
    // Refraction
    bool hasRefrac = transmittedDirection(
        normal, ray.getDirection(),
        refrIndex, nextRefrIndex,
        transmittedDir);

    if (!hasRefrac)
        return Vector3f::ZERO;

    Ray transmittedRay{point, transmittedDir};
    Hit refractHit;
    Vector3f refrColor = traceRay(transmittedRay, refractHit, EPSILON,
                                  bounces - 1, nextRefrIndex);
    return refrColor * hit.getMaterial()->getSpecularColor();
}

Vector3f RayTracer::traceRay(Ray &ray, Hit &hit, float tMin,
                             unsigned int bounces, float refrIndex,
                             Vector3f &normalViz)
{
    if (scene->getGroup()->intersect(ray, hit, tMin))
    {
        // Ambient + Illumination + Blended reflection and refraction
        // Set color to ambient light
        Vector3f color = scene->getAmbientLight() * hit.getMaterial()->getDiffuseColor();
        Vector3f point = ray.pointAtParameter(hit.getT());

        // Shade with illumination
        color += computeIllumination(ray, hit, point, normalViz);

        // No more bounces left or no specular
        if (bounces == 0 || hit.getMaterial()->getSpecularColor() == Vector3f{0})
            return color;

        // Check if ray is inside material
        Vector3f normal = hit.getNormal();
        float nextRefrIndex = hit.getMaterial()->getRefractionIndex();
        // If ray is exiting an object, the normal and ray direction will differ
        // by an angle of 0 to 90 degrees ie. cos(theta) > 0
        if (Vector3f::dot(ray.getDirection(), normal) > 0)
        {
            // Assume that ray will go from material to air
            nextRefrIndex = 1;
            normal = -normal;
        }

        Vector3f transmittedDir;
        // Get reflection and refraction
        Vector3f reflColor = computeReflection(ray, hit, point, bounces, refrIndex, normal);
        Vector3f refrColor = computeRefraction(ray, hit, point, bounces, refrIndex,
                                               nextRefrIndex, normal, transmittedDir);

        // If there is no refraction ie. total internal reflection
        if (refrColor == Vector3f::ZERO)
            return color + reflColor;

        // Compute Schlick's approximation reflectance
        float reflectance = computeReflectance(normal, ray.getDirection(),
                                               refrIndex, nextRefrIndex,
                                               transmittedDir);
        return color + reflectance * reflColor + (1 - reflectance) * refrColor;
    }

    return scene->getBackgroundColor(ray.getDirection());
}

Vector3f RayTracer::traceRay(Ray &ray, Hit &hit, float tMin,
                             unsigned int bounces, float refrIndex)
{
    Vector3f temp;
    return traceRay(ray, hit, tMin, bounces, refrIndex, temp);
}
