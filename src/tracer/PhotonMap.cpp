#include "PhotonMap.h"

#define EPSILON 0.001

PhotonMap::PhotonMap(SceneParser *scene, Arguments *args)
{
    this->scene = scene;
    this->maxPhotons = args->maxPhotons;
    this->maxBounces = args->photonBounces;
    this->numPhotonsPerLight = maxPhotons / scene->getNumLights();

    this->gMapIndex = new PhotonTreeIndex(3, gMap);
    this->cMapIndex = new PhotonTreeIndex(3, cMap);
    this->numStoredPhotons = 0;
}

void PhotonMap::store(Photon p, bool caustic)
{
    if (numStoredPhotons >= maxPhotons)
        return;

    if (caustic)
        cMap.pts.push_back(p);
    else
        gMap.pts.push_back(p);

    numStoredPhotons++;
}

void PhotonMap::store(const Vector3f &pos, const Vector3f &col, const Vector3f &dir, bool caustic)
{
    Photon p;
    p.position = pos;
    p.color = col;
    p.inDirection = dir;

    this->store(p, caustic);
}

void PhotonMap::balance()
{
    // pMapTree.optimize();
}

void PhotonMap::findInRadius(const Vector3f &pos, float radius,
                             vector<pair<size_t, float>> &matches,
                             bool caustic)
{
    matches.clear();

    nanoflann::SearchParams params;
    params.sorted = true;

    if (caustic)
        cMapIndex->radiusSearch(pos, radius, matches, params);
    else
        gMapIndex->radiusSearch(pos, radius, matches, params);
}

void PhotonMap::findKnn(const Vector3f &pos, int num,
                        vector<size_t> &retIndex, vector<float> &dists2,
                        bool caustic)
{
    retIndex = vector<size_t>(num);
    dists2 = vector<float>(num);

    int numResults = num;
    if (caustic)
        numResults = cMapIndex->knnSearch(pos, num, &retIndex[0], &dists2[0]);
    else
        numResults = gMapIndex->knnSearch(pos, num, &retIndex[0], &dists2[0]);

    // In case of less points in the tree than requested:
    retIndex.resize(numResults);
    dists2.resize(numResults);
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
            tracePhoton(photonRay, light->getSourceColor(), maxBounces, 1.0f, 0);
        }
    }

    // this->balance();
    gMapIndex->buildIndex();
    cMapIndex->buildIndex();
}

void PhotonMap::tracePhoton(Ray &photonRay, Vector3f color,
                            unsigned int bounces, float refrIndex,
                            char flag)
{
    if (bounces == 0)
        return;

    Hit hit;
    float tMin = scene->getCamera()->getTMin() + EPSILON;
    if (!scene->getGroup()->intersect(photonRay, hit, tMin))
        return;

    Vector3f point = photonRay.pointAtParameter(hit.getT());
    Vector3f normal = hit.getNormal();

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
                return tracePhoton(tirRay, color * specCol, bounces - 1, refrIndex, flag | 0x2);
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
                        bounces - 1, refrIndex, flag | 0x2);
        }

        // Refract
        float transmissitivity = 1 - reflectance;
        if (transmissitivity > 0.0f)
        {
            Ray refractRay{point, transmittedDir};
            tracePhoton(refractRay, color * specCol * transmissitivity,
                        bounces - 1, nextRefrIndex, flag | 0x2);
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
                               bounces - 1, refrIndex, flag | 0x2);
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
            tracePhoton(reflectRay, difCol * color / Pd,
                        bounces - 1, refrIndex, flag | 0x2);
        }
        else if (roulette < Pd + Ps)
        {
            // Specular reflect
            Vector3f reflectDir = mirrorDirection(normal, photonRay.getDirection());
            Ray reflectRay{point, reflectDir};
            tracePhoton(reflectRay, specCol * color / Ps,
                        bounces - 1, refrIndex, flag | 0x2);
        }

        store(point, color / numPhotonsPerLight, photonRay.getDirection(), flag == 0x2);
    }
    else
    {
        store(point, color / numPhotonsPerLight, photonRay.getDirection(), flag == 0x2);

        /* Non-specular material */
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
            tracePhoton(reflectRay, difCol * color / Pr,
                        bounces - 1, refrIndex, flag | 0x1);
        }
    }
}

Vector3f PhotonMap::knnRadianceEstimate(Ray &ray, Hit &hit, int numRadPhotons, bool caustic)
{
    if (hit.getMaterial()->getRefractionIndex() != 0)
        return Vector3f::ZERO;

    Vector3f point = ray.pointAtParameter(hit.getT());
    vector<size_t> indices;
    vector<float> dists2;
    findKnn(point, numRadPhotons, indices, dists2, caustic);

    if (indices.size() == 0)
        return Vector3f::ZERO;

    Vector3f col = Vector3f::ZERO;
    for (int i = 0; i < indices.size(); i++)
    {
        Photon p;
        if (caustic)
            p = cMap.pts[i];
        else
            p = gMap.pts[i];

        float contrib = Vector3f::dot(p.inDirection, hit.getNormal());
        contrib = std::max(contrib, 0.0f);
        col += contrib * p.color;
    }

    float radius2 = *max_element(begin(dists2), end(dists2));
    return col / (M_PI * radius2);
}

Vector3f PhotonMap::radialRadianceEstimate(Ray &ray, Hit &hit, float radius, bool caustic)
{
    if (hit.getMaterial()->getRefractionIndex() != 0)
        return Vector3f::ZERO;

    Vector3f point = ray.pointAtParameter(hit.getT());
    vector<pair<size_t, float>> matches;
    findInRadius(point, radius, matches, caustic);

    if (matches.size() == 0)
        return Vector3f::ZERO;

    Vector3f col = Vector3f::ZERO;
    for (int i = 0; i < matches.size(); i++)
    {
        Photon p;
        if (caustic)
            p = cMap.pts[i];
        else
            p = gMap.pts[i];

        float contrib = Vector3f::dot(p.inDirection, hit.getNormal());
        contrib = std::max(contrib, 0.0f);
        col += contrib * p.color;
    }

    float radius2 = radius * radius;
    return col / (M_PI * radius2);
}