#ifndef __RAYTRACER_APP__
#define __RAYTRACER_APP__

#include <cstdio>
#include <cmath>
#include <list>
#include <memory>
#include <functional>

#include <Windows.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/GLU.h>

#include "shape.h"
#include "info.h"
#include "collisionresponse.h"

#include "camera.h"
#include "color.h"

#define DEL_PTR(x)	if(x) { delete x; };

class RayTracerApp {

public:
	RayTracerApp(int w, int h);

	~RayTracerApp(void);

	void run(void);

	void sim_window_draw(void);
	void control_window_draw(void);
	
	void sim_step(void);
	void sim_start(void);
	void sim_reset(void);

	bool is_init_ok(void) { return init_ok; };
private:
	void draw_axes(float s);
	void render_image(void);

	bool init_ok;

	bool running = true;

	int window_width, window_height;

	GLFWwindow * sim_window;
	GLFWwindow * control_window;

	const int CONTROL_WIN_WIDTH = 380;

	ImGuiWindowFlags imgui_flags;

	Info * info;

	std::list<std::unique_ptr<Ray>> rays;
	ShapeSet scene;
	Plane * plane; // (Point(0.0f, 0.0f, 0.0f), 1.0f, Vector(0.0f, 1.0f, 0.0f));
	Sphere * sphere; // (Point(0.0f, 1.0f, 0.0f), 1.0f, (4.0f / 3.0f));
	Ellipsoid * ellipsoid;

	bool show_axes, show_objects;
	bool add_sphere, add_ellipsoid;

	int ray_group;
	int fps;
	float start_pos_x, start_pos_y, start_pos_z;
	float alpha, beta, gamma;

	int steps;
	int total_plane_collisions;
	float total_plane_energy;
	int no_rays;
 
	float cam_x, cam_y, cam_z;

	float lamp_width, lamp_height;
	int rays_per_width, rays_per_height;

	int anim_delay;

	int render_width, render_height;
	float center_y;
	float radius_x, radius_y, radius_z;

	int grid_x, grid_z;

	float energy_threshold;
};

#endif
