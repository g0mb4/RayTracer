#ifndef __COLLISION_RESPONSE_H__
#define __COLLISION_RESPONSE_H__

#include "ray.h"
#include "shape.h"
#include "vectormath.h"

class CollisionResponse
{
public:
	CollisionResponse(const Intersection & intersection, Ray * ray, Ray * reflected, Ray * refracted);
	virtual ~CollisionResponse() {};

private:
	void planeCollision(const Plane * p, Point c, Ray * ray, Ray * reflected, Ray * refracted);
	void sphereCollision(const Sphere * s, Point c, Ray * ray, Ray * reflected, Ray * refracted);
	void ellipsoidCollision(const Ellipsoid * s, Point c, Ray * ray, Ray * reflected, Ray * refracted);

	Ray reflect(const Ray * r, const Point& c, const Vector& normal);
};

#endif

