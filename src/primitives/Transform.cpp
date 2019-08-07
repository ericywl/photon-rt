#include "Transform.h"

Transform::Transform(const Matrix4f &m, Object3D *obj)
{
    trans = m;
    transInv = m.inverse();
    transInvT = transInv.transposed();
    o = obj;
}

bool Transform::intersect(const Ray &r, Hit &h, float tmin)
{
    Vector3f orig = (transInv * Vector4f{r.getOrigin(), 1.0f}).xyz();
    Vector3f dir = (transInv * Vector4f{r.getDirection(), 0.0f}).xyz();
    
    // Use transformed ray for intersection computation
    Ray transformedRay = Ray(orig, dir);
    if (o->intersect(transformedRay, h, tmin))
    {
        // Transform normals properly
        Vector3f norm = (transInvT * Vector4f{h.getNormal(), 0.0f}).xyz();
        h.set(h.getT(), h.getMaterial(), norm.normalized());
        return true;
    }
    
    return false;
}