#ifndef CAMERA_H
#define CAMERA_H

#include "vectormath.h"
#include "ray.h"

class Camera
{
public:
	virtual ~Camera() { }

	virtual Ray makeRay(Vector2 point) const = 0;
};

class PerspectiveCamera : public Camera
{
protected:
	Point origin;
	Vector forward;
	Vector up;
	Vector right;

	double h, w;

public:
	PerspectiveCamera(double fov, double aspectRatio, Point origin, Point target, Vector up);
	PerspectiveCamera(Point origin, Vector direction, Vector up, double fov, double aspectRatio);

	virtual Ray makeRay(Vector2 point) const;
};

#endif // CAMERA_H
