#ifndef PHOTONMAP_H
#define PHOTONMAP_H

#include "Ray.h"
#include "SceneParser.h"
#include "Helper.h"

#include "Vector3f.h"
#include "kdtree.hpp"

struct Photon
{
    Vector3f position;
    Vector3f color;
    Vector3f inDirection;

    Photon(){};

    Photon(Vector3f pos, Vector3f col, Vector3f dir) : position(pos),
                                                       color(col),
                                                       inDirection(dir) {}
};

struct KDTreeNode
{
    typedef double value_type;
    Photon p;
    int index;

    KDTreeNode(Photon p)
    {
        this->p = p;
    }

    value_type operator[](int n) const
    {
        return p.position[n];
    }

    float distance(const KDTreeNode &node)
    {
        return (node.p.position - p.position).abs();
    }
};

class PhotonMap
{
public:
    PhotonMap(int maxPhotons);

    void store(Photon p);
    void store(const Vector3f &pos, const Vector3f &power, const Vector3f &dir);
    void balance();

private:
    int maxPhotons;
    KDTree::KDTree<3, KDTreeNode> pMapTree;
};

class PhotonTracer
{
public:
    PhotonTracer(SceneParser *scene, int maxPhotons, int maxBounces);

    void buildPhotonMap(unsigned int numPhotons);

    void tracePhoton(Ray &photonRay, Vector3f color,
                     unsigned int bounces, float refrIndex);

private:
    int maxBounces;
    SceneParser *scene;
    PhotonMap *photonMap;
};

#endif