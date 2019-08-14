#include "PhotonMap.h"

PhotonMap::PhotonMap(SceneParser *scene, int maxPhotons, int maxBounces)
{
    this->scene = scene;
    this->maxPhotons = maxPhotons;
    this->maxBounces = maxBounces;
    this->numStoredPhotons = 0;
    this->numPhotonsPerLight = maxPhotons / scene->getNumLights();
}

void PhotonMap::store(Photon p)
{
    if (numStoredPhotons >= maxPhotons)
        return;

    pMapTree.insert(KDTreeNode{p});
    numStoredPhotons++;
}

void PhotonMap::store(const Vector3f &pos, const Vector3f &col, const Vector3f &dir)
{
    Photon p;
    p.position = pos;
    p.color = col;
    p.inDirection = dir;

    this->store(p);
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
    assert(scene->getNumLights() != 0);
    while (numStoredPhotons < maxPhotons)
    {
        for (unsigned int i = 0; i < scene->getNumLights(); i++)
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

    Vector3f orig = photonRay.getOrigin();
    Vector3f point = photonRay.pointAtParameter(hit.getT());
    Vector3f normal = hit.getNormal();
    float distSq = (orig - point).absSquared();
    color = color / (1 + 5 * distSq);

    Vector3f difCol = hit.getMaterial()->getDiffuseColor();
    Vector3f specCol = hit.getMaterial()->getSpecularColor();
    float nextRefrIndex = hit.getMaterial()->getRefractionIndex();

    Vector3f transmittedDir;
    if (nextRefrIndex > 0)
    {
        /* Dielectric material */
        if (specCol == Vector3f::ZERO)
            return;

        // Check if ray is inside material
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

        // Reflect
        if (reflectance > 0.0f)
        {
            Vector3f reflectDir = mirrorDirection(normal, photonRay.getDirection());
            Ray reflectRay{point, reflectDir};
            tracePhoton(reflectRay, color * specCol * reflectance,
                        bounces - 1, refrIndex);
        }

        // Refract
        float transmissitivity = 1 - reflectance;
        if (transmissitivity > 0.0f)
        {
            Ray refractRay{point, transmittedDir};
            tracePhoton(refractRay, color * specCol * transmissitivity,
                        bounces - 1, nextRefrIndex);
        }
    }
    else if (specCol != Vector3f::ZERO)
    {
        /* Specular material */
        if (difCol == Vector3f::ZERO)
        {
            // Specular reflect
            Vector3f reflectDir = mirrorDirection(normal, photonRay.getDirection());
            Ray reflectRay{point, reflectDir};
            return tracePhoton(reflectRay, color * specCol,
                               bounces - 1, refrIndex);
        }

        Vector3f d = difCol / 2.0f;
        Vector3f s = specCol / 2.0f;
        Vector3f refCol = d + s;

        // Calculate diffuse and specular reflect probabilities
        float Pr = refCol.max();
        float Pd = d.sum() / refCol.sum() * Pr;
        float Ps = Pr - Pd;

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
            tracePhoton(reflectRay, color * difCol,
                        bounces - 1, refrIndex);
        }
        else if (roulette < Pd + Ps)
        {
            // Specular reflect
            Vector3f reflectDir = mirrorDirection(normal, photonRay.getDirection());
            Ray reflectRay{point, reflectDir};
            tracePhoton(reflectRay, color * specCol,
                        bounces - 1, refrIndex);
        }
    }
    else
    {
        /* Non-specular material */
        store(point, color, photonRay.getDirection());

        float Pr = difCol.max();
        float roulette = randomFloat(0, 1);
        if (roulette < Pr)
        {
            // Diffuse reflect
            Vector3f reflectDir = randomUnitVector();
            // Flip direction if not in normal hemisphere
            if (Vector3f::dot(reflectDir, normal) < 0.0f)
                reflectDir = -reflectDir;

            Ray reflectRay{point, reflectDir};
            tracePhoton(reflectRay, color * difCol,
                        bounces - 1, refrIndex);
        }
    }
}

Vector3f PhotonMap::radianceEstimate(Ray &ray, Hit &hit, float radius)
{
    if (hit.getMaterial()->getRefractionIndex() != 0)
        return Vector3f::ZERO;

    Vector3f point = ray.pointAtParameter(hit.getT());
    vector<KDTreeNode> photons = findInRange(point, 2 * radius);

    if (photons.size() == 0)
        return Vector3f::ZERO;

    float radius2 = radius * radius;
    Vector3f col = Vector3f::ZERO;
    for (KDTreeNode n : photons)
    {
        if ((n.p.position - point).absSquared() > radius2)
            continue;

        float contrib = Vector3f::dot(-n.p.inDirection, hit.getNormal());
        contrib = std::max(contrib, 0.0f);
        col += n.p.color;
    }

    return col / (M_PI * radius2) / numPhotonsPerLight;
}