#ifndef PHOTONMAP_H
#define PHOTONMAP_H

#include "Ray.h"
#include "SceneParser.h"
#include "Helper.h"
#include "ArgParser.h"

#include "Vector3f.h"
#include "kdtree.hpp"
#include "nanoflann.hpp"

struct Photon
{
    Vector3f position;
    Vector3f color;
    Vector3f inDirection;

    Photon(){};

    Photon(Vector3f pos, Vector3f col, Vector3f dir)
    {
        position = pos;
        color = col;
        inDirection = dir.normalized();
    }
};

struct PhotonTree
{
    std::vector<Photon> pts;

    // Must return the number of data points
    inline size_t kdtree_get_point_count() const { return pts.size(); }

    inline float kdtree_distance(const Vector3f &p1, const size_t idx_p2, size_t /*size*/) const
    {
        const Vector3f diff = p1 - pts[idx_p2].position;
        return diff.abs();
    }

    // Returns the dim'th component of the idx'th point in the class:
    // Since this is inlined and the "dim" argument is typically an immediate value, the
    //  "if/else's" are actually solved at compile time.
    inline float kdtree_get_pt(const size_t idx, int dim) const
    {
        if (dim == 0)
            return pts[idx].position.x();
        else if (dim == 1)
            return pts[idx].position.y();
        else
            return pts[idx].position.z();
    }

    // Optional bounding-box computation: return false to default to a standard bbox computation loop.
    //   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
    //   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
    template <class BBOX>
    bool kdtree_get_bbox(BBOX & /*bb*/) const { return false; }
};

// construct a kd-tree index:
typedef nanoflann::KDTreeSingleIndexAdaptor<
    nanoflann::L2_Simple_Adaptor<float, PhotonTree>,
    PhotonTree,
    3 /* dim */
    >
    PhotonTreeIndex;

class PhotonMap
{
public:
    PhotonMap(SceneParser *scene, Arguments *args);
    
    bool ready = false;

    void build();

    void store(Photon p, bool caustic);

    void store(const Vector3f &pos, const Vector3f &power, const Vector3f &dir,
               bool caustic);

    void balance();

    void findInRadius(const Vector3f &pos, float radius,
                      vector<std::pair<size_t, float>> &matches,
                      bool caustic);

    void findKnn(const Vector3f &pos, int num,
                 vector<size_t> &retIndex, vector<float> &distSqr,
                 bool caustic);

    void tracePhoton(Ray &photonRay, Vector3f color,
                     unsigned int bounces, float refrIndex,
                     char flag);

    Vector3f knnRadianceEstimate(Ray &ray, Hit &hit, int numRadPhotons,
                                 bool caustic);

    Vector3f radialRadianceEstimate(Ray &ray, Hit &hit, float radius, bool caustic);

private:
    int maxPhotons;
    int maxBounces;
    int numStoredPhotons;
    int numPhotonsPerLight;
    SceneParser *scene;

    PhotonTree gMap;
    PhotonTreeIndex *gMapIndex;
    PhotonTree cMap;
    PhotonTreeIndex *cMapIndex;
};

#endif