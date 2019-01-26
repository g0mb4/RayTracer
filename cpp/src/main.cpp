// https://github.com/Hillsoft/Raytracer-Tutorial

#include <cstdio>
#include <cmath>
#include <list>
#include <chrono>
#include <memory>
#include <thread>

#include <Windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <GL/glui.h>

#include "shape.h"
#include "info.h"
#include "collisionresponse.h"

int window_width = 800, window_height = 600;
bool mouseControl = true;
unsigned char key = 0;
int mx, my;

GLUI_Checkbox   * chbShowAxes, *chbShowObjects;
GLUI_Checkbox   * chbAddSphere;

GLUI_RadioGroup * rRays;
GLUI_Spinner    * sPosx, * sPosy, * sPosz;
GLUI_Spinner    * sAlpha, * sBeta, * sGamma;
GLUI_Spinner    * sLampW, * sLampH, * sRaysW, * sRaysH;
GLUI_Spinner    * sAnimDelay;

int show_axes = 0, show_objects = 1;
int add_sphere = 1;

int ray_group = 0;
int fps;
float start_pos_x = -2.0f, start_pos_y = 1, start_pos_z = 0.0f;
float alpha = 0, beta = 90, gamma = 90;

std::list<std::unique_ptr<Ray>> rays;
ShapeSet scene;
Info info(window_width, window_height);
Plane plane(Point(0.0f, 0.0f, 0.0f), 1.0f, Vector(0.0f, 1.0f, 0.0f));
Sphere sphere(Point(0.0f, 1.0f, 0.0f), 1.0f, (4.0f / 3.0f));

int steps = 0;
int total_plane_collisions = 0;
int no_rays = 0;

float cam_x = 0, cam_y = 1, cam_z = 8;

float lamp_width = 1.9f, lamp_height = 2.0f;
int rays_per_width = 10, rays_per_height = 10;

int anim_delay = 500;

void step(void);
void reset(void);

void control_cb(int control){
	reset();
}

void changeSize(int w, int h)
{
	if (h == 0)
		h = 1;

	float ratio = (float)w / (float)h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, w, h);

	gluPerspective(45, ratio, 1, 1000);

	glMatrixMode(GL_MODELVIEW);
}

void processNormalKeys(unsigned char pressedKey, int x, int y)
{
	switch (pressedKey) {
	case 27:
		glutLeaveMainLoop();
		break;
	case 'w':
	case 'a':
	case 's':
	case 'd':
		key = pressedKey;
		break;
	case ' ':
		step();
		break;
	}

	glutPostRedisplay();
}


void drawAxes(float scale)
{
	glPushMatrix();

	glScalef(scale, scale, scale);

	glColor3f(1.0, 0.0, 0.0); // red x
	glBegin(GL_LINES);
	// x aix

	glVertex3f(-4.0, 0.0f, 0.0f);
	glVertex3f(4.0, 0.0f, 0.0f);

	// arrow
	glVertex3f(4.0, 0.0f, 0.0f);
	glVertex3f(3.0, 1.0f, 0.0f);

	glVertex3f(4.0, 0.0f, 0.0f);
	glVertex3f(3.0, -1.0f, 0.0f);
	glEnd();
	glFlush();

	// y 
	glColor3f(0.0, 1.0, 0.0); // green y
	glBegin(GL_LINES);
	glVertex3f(0.0, -4.0f, 0.0f);
	glVertex3f(0.0, 4.0f, 0.0f);

	// arrow
	glVertex3f(0.0, 4.0f, 0.0f);
	glVertex3f(1.0, 3.0f, 0.0f);

	glVertex3f(0.0, 4.0f, 0.0f);
	glVertex3f(-1.0, 3.0f, 0.0f);
	glEnd();
	glFlush();

	// z 
	glColor3f(0.0, 0.0, 1.0); // blue z
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0f, -4.0f);
	glVertex3f(0.0, 0.0f, 4.0f);

	// arrow
	glVertex3f(0.0, 0.0f, 4.0f);
	glVertex3f(0.0, 1.0f, 3.0f);

	glVertex3f(0.0, 0.0f, 4.0f);
	glVertex3f(0.0, -1.0f, 3.0f);
	glEnd();

	glPopMatrix();
}

void renderScene(void) {
	auto start_time = std::chrono::high_resolution_clock::now();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	gluLookAt(cam_x, cam_y, cam_z, 0, 0, 0, 0, 1, 0);

	if (show_axes) {
		drawAxes(0.1f);
	}
	
	if (show_objects) {
		for (auto s : scene.shapes) {
			s->draw();
		}
	}

	no_rays = 0;
	for (auto &ray : rays) {
		if (ray) {
			ray->draw();
			no_rays++;
		}
	}

	auto end_time = std::chrono::high_resolution_clock::now();
	auto time = end_time - start_time;
	auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();

	fps = (int)(1000.0 / duration_ms);
	info.Refresh();

	key = 0;
	glutSwapBuffers();
	glutPostRedisplay();	
}

void step(void){
	for (auto &ray : rays) {
		if (ray) {
			Intersection intersection(*ray);
			if (scene.intersect(intersection)) {
				bool succ = false;
				CollisionResponse c(intersection, ray.get(), &succ);
				if (intersection.pShape->type == T_PLANE) {
					total_plane_collisions++;
				}

				if (!succ) {
					ray.release();
					// rays.remove(ray); // TODO
				}
			}
			else {
				ray.release();
				//rays.remove(ray); // TODO
			}
		}
	}

	steps++;
}

void start(void) {
	while (no_rays > 0) {
		step();
		renderScene();
		Sleep(anim_delay);
	}
}

void reset(void){
	auto cosd = [](float a) {
		return cos(a * (PI / 180.0f));
	};

	rays.clear();

	// single ray
	if (ray_group == 0) {
		rays.push_back(std::make_unique<Ray>(Point(start_pos_x, start_pos_y, start_pos_z), Vector(cosd(alpha), cosd(beta), cosd(gamma))));
	}
	else {
		float d_y = lamp_width / (float)rays_per_width;
		float d_z = lamp_height / (float)rays_per_height;
		float start_y = start_pos_y - (lamp_width / 2.0f);
		float end_y = start_pos_y + (lamp_width / 2.0f);
		float start_z = start_pos_z - (lamp_height / 2.0f);
		float end_z = start_pos_z + (lamp_height / 2.0f);

		for (float y = start_y; y < end_y; y += d_y) {
			for (float z = start_z; z < end_z; z += d_z) {
				rays.push_back(std::make_unique<Ray>(Point(start_pos_x, y, z), Vector(cosd(alpha), cosd(beta), cosd(gamma))));
			}
		}
	}

	scene.clear();
	scene.addShape(&plane);
	if (add_sphere) {
		scene.addShape(&sphere);
	}

	no_rays = (int)rays.size();
	steps = 0;
	total_plane_collisions = 0;
}

int main(int argc, char *argv[])
{
	info.AddVariable(10, 12, "FPS", (void*)&fps, PT_INT);
	info.AddVariable(10, 24, "MEM", (void*)&fps, PT_INT);	// internal
	info.AddVariable(10, 48, "STEPS", (void*)&steps, PT_INT);
	info.AddVariable(10, 60, "PLANE COLLS", (void*)&total_plane_collisions, PT_INT);
	info.AddVariable(10, 72, "NUMBER OF RAYS", (void*)&no_rays, PT_INT);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("RayTracer");

	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);

	glutKeyboardFunc(processNormalKeys);

	GLUI *glui = GLUI_Master.create_glui("RayTracer", 0, 100 + window_width, 100);
	new GLUI_StaticText(glui, "   RayTracer Control Panel   ");
	new GLUI_Separator(glui);

	GLUI_Panel * vis_panel = new GLUI_Panel(glui, "Visual settings");

	chbShowAxes = new GLUI_Checkbox(vis_panel, "Show axes", &show_axes, 0, control_cb);
	chbShowAxes = new GLUI_Checkbox(vis_panel, "Show objects", &show_objects, 0, control_cb);

	GLUI_Panel * rays_panel = new GLUI_Panel(glui, "Configuraton of rays");
	rRays = new GLUI_RadioGroup(rays_panel, &ray_group, 0, control_cb);
	new GLUI_RadioButton(rRays, "Single ray");
	new GLUI_RadioButton(rRays, "Multiple rays");

	new GLUI_Separator(rays_panel);

	sPosx = new GLUI_Spinner(rays_panel, "Pos X:", &start_pos_x, 0, control_cb);
	sPosy = new GLUI_Spinner(rays_panel, "Pos Y:", &start_pos_y, 0, control_cb);
	sPosz = new GLUI_Spinner(rays_panel, "Pos Z:", &start_pos_z, 0, control_cb);

	new GLUI_Separator(rays_panel);

	sAlpha = new GLUI_Spinner(rays_panel, "Alpha, deg:", &alpha, 0, control_cb);
	sBeta = new GLUI_Spinner(rays_panel,  "Beta, deg:", &beta, 0, control_cb);
	sGamma = new GLUI_Spinner(rays_panel, "Gamma, deg:", &gamma, 0, control_cb);

	new GLUI_Separator(rays_panel);

	sLampW = new GLUI_Spinner(rays_panel, "Width of lamp:", &lamp_width, 0, control_cb);
	sLampH = new GLUI_Spinner(rays_panel, "Height of lamp:", &lamp_height, 0, control_cb);
	sRaysW = new GLUI_Spinner(rays_panel, "Rays per width:", &rays_per_width, 0, control_cb);
	sRaysH = new GLUI_Spinner(rays_panel, "Rays per height:", &rays_per_height, 0, control_cb);

	GLUI_Panel * obj_panel = new GLUI_Panel(glui, "Object settings");
	chbAddSphere = new GLUI_Checkbox(obj_panel, "Sphere", &add_sphere, 0, control_cb);

	GLUI_Panel * sim_panel = new GLUI_Panel(glui, "Simulation");
	new GLUI_Button(sim_panel, "Step", 0, (GLUI_Update_CB)step);
	sAnimDelay = new GLUI_Spinner(sim_panel, "Delay, ms:", &anim_delay, 0, control_cb);
	new GLUI_Button(sim_panel, "Start", 0, (GLUI_Update_CB)start);
	new GLUI_Button(sim_panel, "Reset", 0, (GLUI_Update_CB)reset);

	new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);

	glui->set_main_gfx_window(glutGetWindow());

	GLUI_Master.set_glutIdleFunc(NULL);

	reset();

	glutMainLoop();

	exit(EXIT_SUCCESS);
}
