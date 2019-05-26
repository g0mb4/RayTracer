#include "collisionresponse.h"

CollisionResponse::CollisionResponse(Intersection & intersection, Ray * ray, float refl_index_medium, ShapeSet * scene, Ray * reflected, Ray * refracted){
	switch (intersection.pShape->type) {
	case T_PLANE :
		planeCollision((Plane*)intersection.pShape, intersection.position(), ray, refl_index_medium, scene, reflected, refracted);
		break;
	case T_ELLIPSOID:
		ellipsoidCollision((Ellipsoid*)intersection.pShape, intersection.position(), ray, refl_index_medium, reflected, refracted);
		break;
	}
}

// https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector
Ray CollisionResponse::reflect(const Ray * r, const Point& c, const Vector& normal) {
	Ray ret = *r;

	ret.direction = (r->direction - (2.0f * dot(r->direction, normal)) * normal).normal();
	ret.addPoint(c);

	return ret;
}

// http://cosinekitty.com/raytrace/chapter09_refraction.html
Ray CollisionResponse::refract(const Ray * r, const Point& c, const Vector& normal, float n1, float n2) {
	Ray ret = *r;
	ret.valid = false;

	auto solveQuadratic = [](float a, float b, float c, float * res) {
		float D = (b * b) - 4 * a * c;

		if (D < 0) {
			return false;
		}
		else {
			res[0] = (-b + sqrtf(D)) / (2 * a);
			res[1] = (-b - sqrtf(D)) / (2 * a);

			return true;
		}
	};

	float cos_a1 = dot(r->direction, normal);
	float sin_a1 = 0;

	if (cos_a1 <= -1.0f) {
		cos_a1 = -1.0f;  // clamp to lower limit
		sin_a1 = 0.0f;
	}
	else if (cos_a1 >= +1.0f) {
		cos_a1 = +1.0f;  // clamp to upper limit
		sin_a1 = 0.0f;
	} else {
		sin_a1 = sqrtf(1.0f - cos_a1 * cos_a1);
	}

	float n_ratio = n1 / n2;
	float sin_a2 = n_ratio * sin_a1;

	if (sin_a2 <= -1.0f || sin_a2 >= +1.0f) {
		// Since sin_a2 is outside the bounds -1..+1, then
		// there is no such real angle a2, which in turn
		// means that the ray experiences total internal reflection,
		// so that no refracted ray exists.
	} else {
		float res[2];

		if (!solveQuadratic(1.0f, 2.0f * cos_a1, 1.0f - (1.0f / (n_ratio*n_ratio)), res)) {
			return ret;
		}

		float max_alignment = -1.0f;
		Vector refract_dir;
		for (int k = 0; k < 2; k++) {
			Vector refract_attempt = r->direction + normal * res[k];
			float alignment = dot(r->direction, refract_attempt);
			if (alignment > max_alignment) {
				max_alignment = alignment;
				refract_dir = refract_attempt;
			}
		}

		if (max_alignment <= 0.0f) {
			// Getting here means there is something wrong with the math.
			// Either there were no solutions to the quadratic equation,
			// or all solutions caused the refracted ray to bend 90 degrees
			// or more, which is not possible.
		} else {
			ret.direction = refract_dir.normal(); // TODO: normal() ???
			ret.addPoint(c);
			ret.valid = true;

			// Determine the cosine of the exit angle.
			double cos_a2 = sqrt(1.0 - sin_a2 * sin_a2);
			if (cos_a1 < 0.0)
			{
				// Tricky bit: the polarity of cos_a2 must
				// match that of cos_a1.
				cos_a2 = -cos_a2;
			}

			// Determine what fraction of the light is
			// reflected at the interface.  The caller
			// needs to know this for calculating total
			// reflection, so it is saved in an output parameter.

			// We assume uniform polarization of light,
			// and therefore average the contributions of s-polarized
			// and p-polarized light.
			double Rs = polarizedReflection(n1, n2, cos_a1, cos_a2);
			double Rp = polarizedReflection(n1, n2, cos_a2, cos_a1);
			double reflectionFactor = (Rs + Rp) / 2.0;

			ret.energy = (1.0 - reflectionFactor) * ret.energy;
		}
	}

	return ret;
}

double CollisionResponse::polarizedReflection(double n1, double n2, double cos_a1, double cos_a2) {
	double left = n1 * cos_a1;
	double right = n2 * cos_a2;
	double numer = left - right;
	double denom = left + right;
	denom *= denom;     // square the denominator
	if (denom < 0.000001)
	{
		// Assume complete reflection.
		return 1.0;
	}
	double reflection = (numer*numer) / denom;
	if (reflection > 1.0)
	{
		// Clamp to actual upper limit.
		return 1.0;
	}
	return reflection;
}

void CollisionResponse::planeCollision(Plane * p, Point c, Ray * ray, float refl_index_medium, ShapeSet * scene, Ray * reflected, Ray * refracted) {
	printf("plane collision at (%f, %f, %f)\n", c.x, c.y, c.z);

	float n1 = refl_index_medium;	// dafault: ray outside droplet, source refl = medium

	for (auto s : scene->shapes) {
		if (s->type == T_ELLIPSOID) {
			// ray inside droplet
			if (((Ellipsoid*)s)->isInside(ray->origin)) {
				n1 = ((Ellipsoid*)s)->reflection;	// source refl = droplet
				break;
			}
		}
	}

	*reflected = reflect(ray, c, p->normal);
	*refracted = refract(ray, c, p->normal, n1, p->reflection);

	if (refracted->valid) {
		reflected->energy -= refracted->energy;
	}
}

void CollisionResponse::ellipsoidCollision(Ellipsoid * s, Point c, Ray * ray, float refl_index_medium, Ray * reflected, Ray * refracted) {
	printf("ellipsoid collision at (%f, %f, %f)\n", c.x, c.y, c.z);

	auto distance = [](Point a, Point b) {
		return sqrtf((a.x - b.x) * (a.x - b.x) +
			(a.y - b.y) * (a.y - b.y) +
			(a.z - b.z) * (a.z - b.z));
	};


	float n1 = 1;
	float n2 = 1;

	if (s->isInside(ray->origin)) {
		n1 = s->reflection;
		n2 = refl_index_medium;
	} else {
		n1 = refl_index_medium;
		n2 = s->reflection;
	}

	Vector normal = (s->centre - c).normal();

	*reflected = reflect(ray, c, normal);
	*refracted = refract(ray, c, normal, n1, n2);

	if (refracted->valid) {
		reflected->energy -= refracted->energy;
	}
}

