#ifndef __RAYTRACER_APP__
#define __RAYTRACER_APP__

#include <cstdio>
#include <cmath>
#include <list>
#include <memory>
#include <functional>

#include <Windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <GL/glui.h>

#include "shape.h"
#include "info.h"
#include "collisionresponse.h"

#define DEL_PTR(x)	if(x) { delete x; };

class RayTracerApp {

public:
	static RayTracerApp * instance;

	static RayTracerApp * init(int * argc, char ** argv, int w, int h){

		if (instance == nullptr) {
			instance = new RayTracerApp(argc, argv, w, h);
		}

		return instance;
	}

	static RayTracerApp * get_instance() { return instance; }

	static void destroy() {
		if (instance != nullptr) {
			delete instance;
		}
	}

	void run(void) { glutMainLoop(); };

	void callback_guit_control(int i);
	static void guit_control_wrap(int i) { RayTracerApp::get_instance()->callback_guit_control(i); };

	void callback_glut_display(void);
	static void glut_display_wrap(void) { RayTracerApp::get_instance()->callback_glut_display(); };

	void callback_glut_reshape(int w, int h);
	static void glut_reshape_wrap(int w, int h) { RayTracerApp::get_instance()->callback_glut_reshape(w, h); };

	void callback_glut_idle(void);
	static void glut_idle_wrap(void) { RayTracerApp::get_instance()->callback_glut_idle(); };

	void callback_glut_keyboard(unsigned char pressed_key, int x, int y);
	static void glut_keyboard_wrap(unsigned char pressed_key, int x, int y) { RayTracerApp::get_instance()->callback_glut_keyboard(pressed_key, x, y); };

	void sim_step(void);
	static void sim_step_wrap(void) { RayTracerApp::get_instance()->sim_step(); };

	void sim_start(void);
	static void sim_start_wrap(void) { RayTracerApp::get_instance()->sim_start(); };

	void sim_reset(void);
	static void sim_reset_wrap(void) { RayTracerApp::get_instance()->sim_reset(); };

private:
	RayTracerApp(int * argc, char ** argv, int w, int h);

	~RayTracerApp(void);

	void draw_axes(float s);

	int window_width, window_height;

	Info * info;

	std::list<std::unique_ptr<Ray>> rays;
	ShapeSet scene;
	Plane * plane; // (Point(0.0f, 0.0f, 0.0f), 1.0f, Vector(0.0f, 1.0f, 0.0f));
	Sphere * sphere; // (Point(0.0f, 1.0f, 0.0f), 1.0f, (4.0f / 3.0f));

	GLUI_Checkbox * chbShowAxes, *chbShowObjects;
	GLUI_Checkbox * chbAddSphere;

	GLUI_RadioGroup * rRays;
	GLUI_Spinner * sPosx, *sPosy, *sPosz;
	GLUI_Spinner * sAlpha, *sBeta, *sGamma;
	GLUI_Spinner * sLampW, *sLampH, *sRaysW, *sRaysH;
	GLUI_Spinner * sAnimDelay;

	int show_axes, show_objects;
	int add_sphere;

	int ray_group;
	int fps;
	float start_pos_x, start_pos_y, start_pos_z;
	float alpha, beta, gamma;

	int steps;
	int total_plane_collisions;
	int no_rays;
 
	float cam_x, cam_y, cam_z;

	float lamp_width, lamp_height;
	int rays_per_width, rays_per_height;

	int anim_delay;
};



#endif
