#ifndef __COLLISION_RESPONSE_H__
#define __COLLISION_RESPONSE_H__

#include "ray.h"
#include "shape.h"
#include "vectormath.h"

class CollisionResponse
{
public:
	CollisionResponse(Intersection & intersection, Ray * ray, float refl_index_medium, ShapeSet * scene, Ray * reflected, Ray * refracted);
	virtual ~CollisionResponse() {};

private:
	void planeCollision(Plane * p, Point c, Ray * ray, float refl_index_medium, ShapeSet * scene, Ray * reflected, Ray * refracted);
	void ellipsoidCollision(Ellipsoid * s, Point c, Ray * ray, float refl_index_medium, Ray * reflected, Ray * refracted);

	Ray reflect(const Ray * r, const Point& c, const Vector& normal);
	Ray refract(const Ray * r, const Point& c, const Vector& normal, float n1, float n2);

	double polarizedReflection(double n1, double n2, double cos_a1, double cos_a2);
};

#endif

