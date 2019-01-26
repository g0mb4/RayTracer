#ifndef __COLLISION_RESPONSE_H__
#define __COLLISION_RESPONSE_H__

#include "ray.h"
#include "shape.h"
#include "vectormath.h"

class CollisionResponse
{
public:
	CollisionResponse(const Intersection & intersection, Ray * ray, bool * success);
	virtual ~CollisionResponse() {};

private:
	void planeCollision(const Plane * p, Point c, Ray * ray);
	void sphereCollision(const Sphere * s, Point c, Ray * ray, bool * success);

};

#endif

