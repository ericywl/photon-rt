#ifndef GROUP_H
#define GROUP_H

#include "Object3D.h"
#include "Ray.h"
#include "Hit.h"
#include <iostream>
#include <vector>

using namespace std;

///TODO:
///Implement Group
///Add data structure to store a list of Object*
class Group : public Object3D
{
public:
    Group(){};

    Group(unsigned int numObjects)
    {
        m_objects = vector<Object3D *>{numObjects};
    }

    ~Group(){};

    virtual bool intersect(const Ray &r, Hit &h, float tmin);
    virtual bool intersect(const Ray &r, Hit &h, float tmin, bool stopEarly);

    void addObject(Object3D *obj);

    void addObject(int idx, Object3D *obj);

    int getGroupSize();

private:
    vector<Object3D *> m_objects;
};

#endif
