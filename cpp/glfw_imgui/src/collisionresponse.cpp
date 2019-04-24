#include "collisionresponse.h"

CollisionResponse::CollisionResponse(const Intersection & intersection, Ray * ray, Ray * reflected, Ray * refracted){
	switch (intersection.pShape->type) {
	case T_PLANE :
		planeCollision((Plane*)intersection.pShape, intersection.position(), ray, reflected, refracted);
		break;
	case T_SPHERE:
		sphereCollision((Sphere*)intersection.pShape, intersection.position(), ray, reflected, refracted);
		break;
	case T_ELLIPSOID:
		ellipsoidCollision((Ellipsoid*)intersection.pShape, intersection.position(), ray, reflected, refracted);
		break;
	}
}

// https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector
Ray CollisionResponse::reflect(const Ray * r, const Point& c, const Vector& normal) {
	Ray ret = *r;

	ret.direction = r->direction - (2.0f * dot(r->direction, normal)) * normal;
	ret.addPoint(c);

	return ret;
}


void CollisionResponse::planeCollision(const Plane * p, Point c, Ray * ray, Ray * reflected, Ray * refracted) {
	printf("plane collision at (%f, %f, %f)\n", c.x, c.y, c.z);

	*reflected = reflect(ray, c, p->normal);
	*refracted = *ray;

	refracted->valid = false;
}

// only refraction!!!
// http://cosinekitty.com/raytrace/chapter09_refraction.html
void CollisionResponse::sphereCollision(const Sphere * s, Point c, Ray * ray, Ray * reflected, Ray * refracted) {
	printf("sphere collision at (%f, %f, %f)\n", c.x, c.y, c.z);

	auto distance = [](Point a, Point b) {
		return sqrtf((a.x - b.x) * (a.x - b.x) + 
			         (a.y - b.y) * (a.y - b.y) +
			         (a.z - b.z) * (a.z - b.z));
	};

	auto solveQuadratic = [](float a, float b, float c, float * res) {
		float D = (b * b) - 4 * a * c;

		if (D < 0) {
			return false;
		} else {
			res[0] = (-b + sqrtf(D)) / (2 * a);
			res[1] = (-b - sqrtf(D)) / (2 * a);

			return true;
		}
	};

	float n1 = 1;
	float n2 = 1;

	// from outside
	if (distance(ray->origin, s->centre) > s->radius) {
		n1 = 1.0f;				// air
		n2 = s->reflection;
	} else {
		n1 = s->reflection;
		n2 = 1.0f;
	}

	Vector normal = (s->centre - c).normal();
	float cos_a1 = dot(ray->direction, normal);
	float sin_a1 = 0;

	if (cos_a1 <= -1.0f) {
		cos_a1 = -1.0f;  // clamp to lower limit
		sin_a1 = 0.0f;
	}
	else if (cos_a1 >= +1.0f) {
		cos_a1 = +1.0f;  // clamp to upper limit
		sin_a1 = 0.0f;
	}
	else {
		sin_a1 = sqrtf(1.0f - cos_a1 * cos_a1);
	}

	float n_ratio = n1 / n2;
	float sin_a2 = n_ratio * sin_a1;

	if (sin_a2 <= -1.0f || sin_a2 >= +1.0f) {
		// Since sin_a2 is outside the bounds -1..+1, then
		// there is no such real angle a2, which in turn
		// means that the ray experiences total internal reflection,
		// so that no refracted ray exists.

		ray->direction = ray->direction - (2.0f * dot(ray->direction, normal)) * normal;
		ray->addPoint(c);
		return;
	}
	else {
		float res[2];

		if (!solveQuadratic(1.0f, 2.0f * cos_a1, 1.0f - (1.0f / (n_ratio*n_ratio)), res)) {
			return;
		}

		float max_alignment = -0.0001f;
		Vector refract_dir;
		for(int k = 0; k < 2; k++) {
			Vector refract_attempt = ray->direction + normal * res[k];
			float alignment = dot(ray->direction, refract_attempt);
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
			return;
		}
		else {
			ray->direction = refract_dir;
			ray->addPoint(c);
		}
	}
}

void CollisionResponse::ellipsoidCollision(const Ellipsoid * s, Point c, Ray * ray, Ray * reflected, Ray * refracted) {
	printf("ellipsoid collision at (%f, %f, %f)\n", c.x, c.y, c.z);

	auto distance = [](Point a, Point b) {
		return sqrtf((a.x - b.x) * (a.x - b.x) +
			(a.y - b.y) * (a.y - b.y) +
			(a.z - b.z) * (a.z - b.z));
	};

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


	float n1 = 1;
	float n2 = 1;

	// TODO: from outside
	if (distance(ray->origin, s->centre) > distance(Point(0, 0, 0), s->radius)) {
		n1 = 1.0f;				// air
		n2 = s->reflection;
	}
	else {
		n1 = s->reflection;
		n2 = 1.0f;
	}

	Vector normal = (s->centre - c).normal();

	*reflected = reflect(ray, c, normal);
	*refracted = *ray;

	refracted->valid = false;

	float cos_a1 = dot(ray->direction, normal);
	float sin_a1 = 0;

	if (cos_a1 <= -1.0f) {
		cos_a1 = -1.0f;  // clamp to lower limit
		sin_a1 = 0.0f;
	}
	else if (cos_a1 >= +1.0f) {
		cos_a1 = +1.0f;  // clamp to upper limit
		sin_a1 = 0.0f;
	}
	else {
		sin_a1 = sqrtf(1.0f - cos_a1 * cos_a1);
	}

	float n_ratio = n1 / n2;
	float sin_a2 = n_ratio * sin_a1;

	if (sin_a2 <= -1.0f || sin_a2 >= +1.0f) {
		// Since sin_a2 is outside the bounds -1..+1, then
		// there is no such real angle a2, which in turn
		// means that the ray experiences total internal reflection,
		// so that no refracted ray exists.
		return;
	}
	else {
		float res[2];

		if (!solveQuadratic(1.0f, 2.0f * cos_a1, 1.0f - (1.0f / (n_ratio*n_ratio)), res)) {
			return;
		}

		float max_alignment = -0.0001f;
		Vector refract_dir;
		for (int k = 0; k < 2; k++) {
			Vector refract_attempt = ray->direction + normal * res[k];
			float alignment = dot(ray->direction, refract_attempt);
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
		}
		else {
			refracted->valid = true;
			refracted->direction = refract_dir;
			refracted->addPoint(c);
		}
	}
}

