#include "Group.h"

bool Group::intersect(const Ray &r, Hit &h, float tmin, bool stopEarly)
{
    bool hasIntersection = false;
    for (Object3D *obj : m_objects)
    {
        if (obj->intersect(r, h, tmin))
        {
            if (stopEarly)
                return true;

            hasIntersection = true;
        }
    }

    return hasIntersection;
}

bool Group::intersect(const Ray &r, Hit &h, float tmin)
{
    return intersect(r, h, tmin, false);
}

void Group::addObject(Object3D *obj)
{
    m_objects.push_back(obj);
}

void Group::addObject(int idx, Object3D *obj)
{
    m_objects[idx] = obj;
}

int Group::getGroupSize()
{
    return m_objects.size();
}