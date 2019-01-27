#include "shape.h"

void ShapeSet::addShape(Shape* shape)
{
	shapes.push_back(shape);
}

void ShapeSet::clear(void) {
	shapes.clear();
}

bool ShapeSet::intersect(Intersection& intersection)
{
	bool doesIntersect = false;

	for (std::vector<Shape*>::iterator iter = shapes.begin();
		iter != shapes.end();
		++iter)
	{
		Shape *curShape = *iter;
		if (curShape->intersect(intersection))
			doesIntersect = true;
	}

	return doesIntersect;
}

bool ShapeSet::doesIntersect(const Ray& ray)
{
	for (std::vector<Shape*>::iterator iter = shapes.begin();
		iter != shapes.end();
		++iter)
	{
		Shape *curShape = *iter;
		if (curShape->doesIntersect(ray))
			return true;
	}

	return false;
}


Plane::Plane(const Point& position, float size, const Vector& normal)
	: position(position), normal(normal), _size(size)
{
	type = T_PLANE;

	/* TODO: 
		_t = (position - p0).normalize();
	*/

	_t = Vector(1.0, 0, 0),
	_b = cross(_t, normal);

	_p1 = position - _t * _size - _b * _size;
	_p2 = position + _t * _size - _b * _size;
	_p3 = position + _t * _size + _b * _size;
	_p4 = position - _t * _size + _b * _size;
}

bool Plane::intersect(Intersection& intersection)
{
	// First, check if we intersect
	float dDotN = dot(intersection.ray.direction, normal);

	if (dDotN == 0.0f)
	{
		// We just assume the ray is not embedded in the plane
		return false;
	}

	// Find point of intersection
	float t = dot(position - intersection.ray.origin, normal) / dDotN;

	if (t <= RAY_T_MIN || t >= intersection.t)
	{
		// Outside relevant range
		return false;
	}

	intersection.t = t;
	intersection.pShape = this;

	return true;
}

bool Plane::doesIntersect(const Ray& ray)
{
	// First, check if we intersect
	float dDotN = dot(ray.direction, normal);

	if (dDotN == 0.0f)
	{
		// We just assume the ray is not embedded in the plane
		return false;
	}

	// Find point of intersection
	float t = dot(position - ray.origin, normal) / dDotN;

	if (t <= RAY_T_MIN || t >= ray.tMax)
	{
		// Outside relevant range
		return false;
	}

	return true;
}

void Plane::draw(void) {
	
	glBegin(GL_LINES);
		glColor3f(0.0f, 1.0f, 0.0f);

		glVertex3f(_p1.x, _p1.y, _p1.z);
		glVertex3f(_p2.x, _p2.y, _p2.z);

		glVertex3f(_p2.x, _p2.y, _p2.z);
		glVertex3f(_p3.x, _p3.y, _p3.z);

		glVertex3f(_p3.x, _p3.y, _p3.z);
		glVertex3f(_p4.x, _p4.y, _p4.z);

		glVertex3f(_p4.x, _p4.y, _p4.z);
		glVertex3f(_p1.x, _p1.y, _p1.z);
	glEnd();
}


Sphere::Sphere(const Point& centre, float radius, float reflection)
	: centre(centre), radius(radius), reflection(reflection)
{
	type = T_SPHERE;
}


bool Sphere::intersect(Intersection& intersection)
{
	// Transform ray so we can consider origin-centred sphere
	Ray localRay = intersection.ray;
	localRay.origin -= centre;

	// Calculate quadratic coefficients
	float a = localRay.direction.length2();
	float b = 2 * dot(localRay.direction, localRay.origin);
	float c = localRay.origin.length2() - sqr(radius);

	// Check whether we intersect
	float discriminant = sqr(b) - 4 * a * c;

	if (discriminant < 0.0f)
	{
		return false;
	}

	// Find two points of intersection, t1 close and t2 far
	float t1 = (-b - std::sqrt(discriminant)) / (2 * a);
	float t2 = (-b + std::sqrt(discriminant)) / (2 * a);

	// First check if close intersection is valid
	if (t1 > RAY_T_MIN && t1 < intersection.t)
	{
		intersection.t = t1;
	}
	else if (t2 > RAY_T_MIN && t2 < intersection.t)
	{
		intersection.t = t2;
	}
	else
	{
		// Neither is valid
		return false;
	}

	// Finish populating intersection
	intersection.pShape = this;

	return true;
}

bool Sphere::doesIntersect(const Ray& ray)
{
	// Transform ray so we can consider origin-centred sphere
	Ray localRay = ray;
	localRay.origin -= centre;

	// Calculate quadratic coefficients
	float a = localRay.direction.length2();
	float b = 2 * dot(localRay.direction, localRay.origin);
	float c = localRay.origin.length2() - sqr(radius);

	// Check whether we intersect
	float discriminant = sqr(b) - 4 * a * c;

	if (discriminant < 0.0f)
	{
		return false;
	}

	// Find two points of intersection, t1 close and t2 far
	float t1 = (-b - std::sqrt(discriminant)) / (2 * a);
	if (t1 > RAY_T_MIN && t1 < ray.tMax)
		return true;

	float t2 = (-b + std::sqrt(discriminant)) / (2 * a);
	if (t2 > RAY_T_MIN && t2 < ray.tMax)
		return true;

	return false;
}

void Sphere::draw(void) {
	glPushMatrix();
		glColor3f(0.0, 0.0, 1.0);
		glTranslatef(centre.x, centre.y, centre.z);
		glScalef(radius, radius, radius);
		glutWireSphere(1.0, 20, 20);
	glPopMatrix();
}
