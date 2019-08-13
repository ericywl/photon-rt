#include "PhotonMap.h"

/* PHOTON MAP */
PhotonMap::PhotonMap(int maxPhotons)
{
    this->maxPhotons = maxPhotons;
}

void PhotonMap::store(Photon p)
{
    pMapTree.insert(KDTreeNode{p});
}

void PhotonMap::store(const Vector3f &pos, const Vector3f &col, const Vector3f &dir)
{
    Photon p;
    p.position = pos;
    p.color = col;
    p.inDirection = dir;

    pMapTree.insert(KDTreeNode{p});
}

void PhotonMap::balance()
{
    pMapTree.optimize();
}

/* PHOTON TRACER */
PhotonTracer::PhotonTracer(SceneParser *scene, int maxPhotons)
{
    this->scene = scene;
    this->photonMap = new PhotonMap(maxPhotons);
}

void PhotonTracer::buildPhotonMap()
{
    for (int i = 0; i < scene->getNumLights(); i++)
    {
        Light *light = scene->getLight(i);
        Vector3f dir = randomUnitVector();
        Ray photonRay{light->getPosition(), dir};
    }
}

void PhotonTracer::tracePhoton(Ray &photonRay, Vector3f color,
                               unsigned int bounces, float refrIndex)
{
    if (bounces == 0)
        return;

    Hit hit;
    float tMin = scene->getCamera()->getTMin();
    if (!scene->getGroup()->intersect(photonRay, hit, tMin))
        return;

    Vector3f point = photonRay.pointAtParameter(hit.getT());

    // Check if ray is inside material
    Vector3f normal = hit.getNormal();
    float nextRefrIndex = hit.getMaterial()->getRefractionIndex();

    Vector3f transmittedDir;
    if (nextRefrIndex > 0)
    {
        // Dielectric material

        // If ray is exiting an object, the normal and ray direction will differ
        // by an angle of 0 to 90 degrees ie. cos(theta) > 0
        if (Vector3f::dot(photonRay.getDirection(), normal) > 0)
        {
            // Assume that ray will go from material to air
            nextRefrIndex = 1;
            normal = -normal;

            bool hasRefrac = transmittedDirection(normal, photonRay.getDirection(),
                                                  refrIndex, nextRefrIndex, transmittedDir);

            if (!hasRefrac)
            {
                // Total internal reflection
                Vector3f tirDir = mirrorDirection(normal, photonRay.getDirection());
                Ray tirRay{point, tirDir};
                return tracePhoton(tirRay, color, bounces - 1, refrIndex);
            }
        }
        else
        {
            transmittedDirection(normal, photonRay.getDirection(),
                                 refrIndex, nextRefrIndex, transmittedDir);
        }

        float reflectance = computeReflectance(normal, photonRay.getDirection(),
                                               refrIndex, nextRefrIndex, transmittedDir);
        // Russian Roulette
        Vector3f rayDir;
        float reflectProb = randomFloat(0, 1);
        if (reflectProb < reflectance)
        {
            // Reflect
            rayDir = mirrorDirection(normal, photonRay.getDirection());
            Ray nextRay{point, rayDir};
            return tracePhoton(nextRay, color, bounces - 1, refrIndex);
        }
    
        // Refract
        rayDir = transmittedDir;
        Ray nextRay{point, rayDir};
        return tracePhoton(nextRay, color, bounces - 1, nextRefrIndex);
    }
    
    Vector3f hitCol = hit.hasTex ? hit.getMaterial()->getTexel(hit.texCoord) : Vector3f{1};
    photonMap->store(point, color * hitCol, photonRay.getDirection());
    
    float colMax = max(color.x(), color.y(), color.z());
    Vector3f difCol = color * hit.getMaterial()->getDiffuseColor();
    Vector3f specCol = color * hit.getMaterial()->getSpecularColor();
    float Pd = max(difCol.x(), difCol.y(), difCol.z()) / colMax;
    float Ps = max(specCol.x(), specCol.y(), specCol.z()) / colMax;
    
    // Russian Roulette
    float roulette = randomFloat(0, 1);
    if (roulette < Pd)
    {
        // Diffuse reflect
        Vector3f reflectDir = randomUnitVector();
        // Flip direction if not in normal hemisphere
        if (Vector3f::dot(reflectDir, normal) < 0.0f)
            reflectDir = -reflectDir;
            
        Ray reflectRay{point, reflectDir};
        return tracePhoton(reflectRay, hitCol * difCol / Pd, bounces - 1, refrIndex);
    }
    
    // Specular reflect
    Vector3f reflectDir = mirrorDirection(normal, photonRay.getDirection());
    Ray reflectRay{point, reflectDir};
    return tracePhoton(reflectRay, specCol / Ps, bounces - 1, refrIndex);
}