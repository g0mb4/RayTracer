#include "shape.h"

void ShapeSet::addShape(Shape* shape)
{
	shapes.push_back(shape);
}

void ShapeSet::clear(void) {
	// TODO:
	/*for (int i = 0; i < shapes.size(); i++) {
		if (shapes[i]) {
			delete shapes[i];
		}
	}*/

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


Plane::Plane(const Point& position, float size, const Vector& normal, float reflection, const Color& c)
	: position(position), normal(normal), _size(size), reflection(reflection)
{
	type = T_PLANE;
	color = c;

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

	if (t <= RAY_T_MIN || t >= RAY_T_MAX)
	{
		// Outside relevant range
		return false;
	}

	return true;
}

void Plane::draw(void) {
	
	glBegin(GL_LINES);
		glColor3f(color.r, color.g, color.b);

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


Ellipsoid::Ellipsoid(const Point& centre, const Point& radius, float reflection, const Color& c)
	: centre(centre), radius(radius), reflection(reflection)
{
	type = T_ELLIPSOID;
	color = c;

	quad = gluNewQuadric();
}

bool Ellipsoid::intersect(Intersection& intersection)
{
	// Transform ray so we can consider origin-centred sphere
	Ray localRay = intersection.ray;
	localRay.origin -= centre;

	// Calculate quadratic coefficients
	float a = ((localRay.direction.x * localRay.direction.x) / (radius.x * radius.x))
			+ ((localRay.direction.y * localRay.direction.y) / (radius.y * radius.y))
			+ ((localRay.direction.z * localRay.direction.z) / (radius.z * radius.z));

	float b = ((2 * localRay.origin.x * localRay.direction.x) / (radius.x * radius.x))
			+ ((2 * localRay.origin.y * localRay.direction.y) / (radius.y * radius.y))
			+ ((2 * localRay.origin.z * localRay.direction.z) / (radius.z * radius.z));

	float c = ((localRay.origin.x * localRay.origin.x) / (radius.x * radius.x))
			+ ((localRay.origin.y * localRay.origin.y) / (radius.y * radius.y))
			+ ((localRay.origin.z * localRay.origin.z) / (radius.z * radius.z))
			- 1;

	// Check whether we intersect
	float discriminant = b * b - 4 * a * c;

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

bool Ellipsoid::doesIntersect(const Ray& ray)
{
	// Transform ray so we can consider origin-centred sphere
	Ray localRay = ray;
	localRay.origin -= centre;

	// Calculate quadratic coefficients
	float a = ((localRay.direction.x * localRay.direction.x) / (radius.x * radius.x))
			+ ((localRay.direction.y * localRay.direction.y) / (radius.y * radius.y))
			+ ((localRay.direction.z * localRay.direction.z) / (radius.z * radius.z));

	float b = ((2 * localRay.origin.x * localRay.direction.x) / (radius.x * radius.x))
			+ ((2 * localRay.origin.y * localRay.direction.y) / (radius.y * radius.y))
			+ ((2 * localRay.origin.z * localRay.direction.z) / (radius.z * radius.z));

	float c = ((localRay.origin.x * localRay.origin.x) / (radius.x * radius.x))
			+ ((localRay.origin.y * localRay.origin.y) / (radius.y * radius.y))
			+ ((localRay.origin.z * localRay.origin.z) / (radius.z * radius.z))
			- 1;

	// Check whether we intersect
	float discriminant = b * b - 4 * a * c;

	if (discriminant < 0.0f)
	{
		return false;
	}

	// Find two points of intersection, t1 close and t2 far
	float t1 = (-b - std::sqrt(discriminant)) / (2 * a);
	if (t1 > RAY_T_MIN && t1 < RAY_T_MAX)
		return true;

	float t2 = (-b + std::sqrt(discriminant)) / (2 * a);
	if (t2 > RAY_T_MIN && t2 < RAY_T_MAX)
		return true;

	return false;
}

void Ellipsoid::draw(void) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// for wireframed
	glPushMatrix();
		glColor3f(color.r, color.g, color.b);
		glTranslatef(centre.x, centre.y, centre.z);
		glScalef(radius.x, radius.y, radius.z);
		gluSphere(quad, 1.0, 20, 20);
	glPopMatrix();
}

bool Ellipsoid::isInside(const Point p) {
	float a = ((p.x - centre.x) / radius.x) * ((p.x - centre.x) / radius.x);
	float b = ((p.y - centre.y) / radius.y) * ((p.y - centre.y) / radius.y);
	float c = ((p.z - centre.z) / radius.z) * ((p.z - centre.z) / radius.z);

	return (a + b + c) < 1.0f;
}

Cube::Cube(const Point& centre, float sideLength, const Color& c)
	: centre(centre)
{
	type = T_CUBE;

	halfSideLength = sideLength / 2.0f;
	color = c;
}

void Cube::draw(void) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// for filled

	glBegin(GL_QUADS);
	glColor3f(color.r, color.g, color.b);

	// front face
	glVertex3f(centre.x - halfSideLength, centre.y + halfSideLength, centre.z + halfSideLength); // top left
	glVertex3f(centre.x + halfSideLength, centre.y + halfSideLength, centre.z + halfSideLength); // top right
	glVertex3f(centre.x + halfSideLength, centre.y - halfSideLength, centre.z + halfSideLength); // bottom right
	glVertex3f(centre.x - halfSideLength, centre.y - halfSideLength, centre.z + halfSideLength); // bottom left

	// back face                                                                                    
	glVertex3f(centre.x - halfSideLength, centre.y + halfSideLength, centre.z - halfSideLength); // top left
	glVertex3f(centre.x + halfSideLength, centre.y + halfSideLength, centre.z - halfSideLength); // top right
	glVertex3f(centre.x + halfSideLength, centre.y - halfSideLength, centre.z - halfSideLength); // bottom right
	glVertex3f(centre.x - halfSideLength, centre.y - halfSideLength, centre.z - halfSideLength); // bottom left

	// left face                                                                                    
	glVertex3f(centre.x - halfSideLength, centre.y + halfSideLength, centre.z + halfSideLength); // top left
	glVertex3f(centre.x - halfSideLength, centre.y + halfSideLength, centre.z - halfSideLength); // top right
	glVertex3f(centre.x - halfSideLength, centre.y - halfSideLength, centre.z - halfSideLength); // bottom right
	glVertex3f(centre.x - halfSideLength, centre.y - halfSideLength, centre.z + halfSideLength); // bottom left

	// right face                                                                                   
	glVertex3f(centre.x + halfSideLength, centre.y + halfSideLength, centre.z + halfSideLength); // top left
	glVertex3f(centre.x + halfSideLength, centre.y + halfSideLength, centre.z - halfSideLength); // top right
	glVertex3f(centre.x + halfSideLength, centre.y - halfSideLength, centre.z - halfSideLength); // bottom right
	glVertex3f(centre.x + halfSideLength, centre.y - halfSideLength, centre.z + halfSideLength); // bottom left

	// top face                                                                                     
	glVertex3f(centre.x - halfSideLength, centre.y + halfSideLength, centre.z + halfSideLength); // top left
	glVertex3f(centre.x - halfSideLength, centre.y + halfSideLength, centre.z - halfSideLength); // top right
	glVertex3f(centre.x + halfSideLength, centre.y + halfSideLength, centre.z - halfSideLength); // bottom right
	glVertex3f(centre.x + halfSideLength, centre.y + halfSideLength, centre.z + halfSideLength); // bottom left

	// top face                                                                                     
	glVertex3f(centre.x - halfSideLength, centre.y - halfSideLength, centre.z + halfSideLength); // top left
	glVertex3f(centre.x - halfSideLength, centre.y - halfSideLength, centre.z - halfSideLength); // top right
	glVertex3f(centre.x + halfSideLength, centre.y - halfSideLength, centre.z - halfSideLength); // bottom right
	glVertex3f(centre.x + halfSideLength, centre.y - halfSideLength, centre.z + halfSideLength); // bottom left

	glEnd();
}
