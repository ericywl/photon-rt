#ifndef PLANE_H
#define PLANE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>

using namespace std;

class Plane : public Object3D
{
public:
	Plane(){};
	Plane(const Vector3f &normal, float d, Material *m);
	Plane(const Vector3f &normal, float d, Material *m, Vector2f textureScale);
	~Plane(){};

	virtual bool intersect(const Ray &r, Hit &h, float tmin);

protected:
	Vector3f normal;
	float d;
	Vector2f textureScale;
};
#endif //PLANE_H
