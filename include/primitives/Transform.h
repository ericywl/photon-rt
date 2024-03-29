#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "Object3D.h"

class Transform : public Object3D
{
public:
    Transform(const Matrix4f &m, Object3D *obj);

    ~Transform() {}

    virtual bool intersect(const Ray &r, Hit &h, float tmin);

protected:
    Matrix4f trans;
    Matrix4f transInv;
    Matrix4f transInvT;
    Object3D *o; //un-transformed object
};

#endif //TRANSFORM_H
