#include "PhotonMap.h"

PhotonMap::PhotonMap(SceneParser *scene, int maxPhotons, int maxBounces)
{
    this->scene = scene;
    this->maxPhotons = maxPhotons;
    this->maxBounces = maxBounces;
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

vector<KDTreeNode> PhotonMap::findInRange(const Vector3f &point, const float radius)
{
    KDTreeNode refNode;
    refNode.p.position = point;

    vector<KDTreeNode> nearbyPhotons;
    pMapTree.find_within_range(refNode, radius,
                               back_insert_iterator<vector<KDTreeNode>>(nearbyPhotons));
    return nearbyPhotons;
}


void PhotonMap::build()
{
    int numPhotons = maxPhotons / scene->getNumLights();
    for (unsigned int i = 0; i < scene->getNumLights(); i++)
    {
        for (unsigned int j = 0; j < numPhotons; j++)
        {
            Light *light = scene->getLight(i);
            Vector3f dir = randomUnitVector();
            Ray photonRay{light->getPosition(), dir};
            tracePhoton(photonRay, light->getSourceColor(), maxBounces, 1.0f);
        }
    }
    
    this->balance();
}

void PhotonMap::tracePhoton(Ray &photonRay, Vector3f color,
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

    // TODO: Should texture be separated with diffuse color?
    ShadeArgs empty;
    // Get possible texture color
    Vector3f difCol = color * hit.getMaterial()->diffuseShade(photonRay, hit, normal, empty, true);
    Vector3f specCol = color * hit.getMaterial()->getSpecularColor();
    store(point, difCol, photonRay.getDirection());

    // Calculate diffuse and specular reflection coefficients
    float colMax = color.max();
    float Pd = difCol.max() / colMax;
    float Ps = specCol.max() / colMax;

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
        return tracePhoton(reflectRay, difCol / Pd, bounces - 1, refrIndex);
    }

    // Specular reflect
    Vector3f reflectDir = mirrorDirection(normal, photonRay.getDirection());
    Ray reflectRay{point, reflectDir};
    // TODO: Why multiply by Pd or Ps?
    return tracePhoton(reflectRay, specCol / Ps, bounces - 1, refrIndex);
}

Vector3f PhotonMap::radianceEstimate(Ray &ray, Hit &hit, float radius)
{
    Vector3f point = ray.pointAtParameter(hit.getT());
    vector<KDTreeNode> photons = findInRange(point, radius);
    Vector3f col = Vector3f::ZERO;
    for (KDTreeNode n : photons)
    {
        Photon p = n.p;
        col = col + p.color;
    }
    
    return col / photons.size();
}