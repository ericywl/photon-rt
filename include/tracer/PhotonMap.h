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
    typedef float value_type;
    Photon p;
    int index;

    KDTreeNode() {}

    KDTreeNode(Photon p)
    {
        this->p = p;
    }

    Vector3f position() const
    {
        return p.position;
    }

    value_type operator[](unsigned int n) const
    {
        assert(n < 3);
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
    int maxPhotons;

    PhotonMap(SceneParser *scene, int maxPhotons, int maxBounces);

    void build();

    void store(Photon p);

    void store(const Vector3f &pos, const Vector3f &power, const Vector3f &dir);

    void balance();

    vector<KDTreeNode> findInRange(const Vector3f &pos, const float radius);

    void tracePhoton(Ray &photonRay, Vector3f color,
                     unsigned int bounces, float refrIndex);

    Vector3f radianceEstimate(Ray &ray, Hit &hit, float radius);

private:
    int maxBounces;
    KDTree::KDTree<3, KDTreeNode> pMapTree;
    SceneParser *scene;
};

#endif