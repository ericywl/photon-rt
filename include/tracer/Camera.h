#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"
#include <vecmath.h>
#include <float.h>
#include <cmath>

class Camera
{
public:
	//generate rays for each screen-space coordinate
	virtual Ray generateRay(const Vector2f &pixel) = 0;

	virtual float getTMin() const = 0;
	virtual ~Camera() {};
	
	float getAspect()
	{
		return aspect;
	}
	
	void setAspect(float newAspect)
	{
		aspect = newAspect;
	}

protected:
	Vector3f center, direction, up, horizontal;
	float angle;
	float aspect = 1.0f;
};

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, float angle);

	virtual Ray generateRay(const Vector2f &pixel);

	virtual float getTMin() const;

private:
	Vector3f u, v, w;
	float D;
};

#endif //CAMERA_H
