#ifndef SPHERE_H
#define SPHERE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
#include <iostream>

using namespace std;

class Sphere : public Object3D
{
public:
	Sphere();
	Sphere(Vector3f center, float radius, Material *material);

	~Sphere() {}

	virtual bool intersect(const Ray &r, Hit &h, float tmin);

protected:
	Vector3f center;
	float radius;
};

#endif
