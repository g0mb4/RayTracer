#include "raytracerapp.h"

RayTracerApp * RayTracerApp::instance = nullptr;

RayTracerApp::RayTracerApp(int * argc, char ** argv, int w, int h) {

	window_width = w;
	window_height = h;

	info = new Info(window_width, window_height);

	info->AddVariable(10, 12, "FPS", (void*)NULL);  // internal
	info->AddVariable(10, 24, "MEM", (void*)NULL);	// internal
	info->AddVariable(10, 48, "STEPS", &steps);
	info->AddVariable(10, 60, "PLANE COLLS", &total_plane_collisions);
	info->AddVariable(10, 72, "NUMBER OF RAYS", &no_rays);

	info->AddVariable(10, 96, "esc", "close program");
	info->AddVariable(10, 108, "spcae", "step sim");
	info->AddVariable(10, 120, "s", "start sim");
	info->AddVariable(10, 132, "r", "reset sim");

	plane = new Plane(Point(0.0f, 0.0f, 0.0f), 1.0f, Vector(0.0f, 1.0f, 0.0f));
	sphere = new Sphere(Point(0.0f, 1.0f, 0.0f), 1.0f, (4.0f / 3.0f));

	show_axes = 0, show_objects = 1;
	add_sphere = 1;

	ray_group = 0;
	start_pos_x = -2.0f, start_pos_y = 1, start_pos_z = 0.0f;
	alpha = 0, beta = 90, gamma = 90;

	steps = 0;
	total_plane_collisions = 0;
	no_rays = 0;

	cam_x = 0, cam_y = 1, cam_z = 8;

	lamp_width = 1.9f, lamp_height = 2.0f;
	rays_per_width = 10, rays_per_height = 10;

	anim_delay = 500;

	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(window_width, window_height);
	GLint win_main = glutCreateWindow("RayTracer");

	glutDisplayFunc(glut_display_wrap);
	glutReshapeFunc(glut_reshape_wrap);
	glutIdleFunc(glut_idle_wrap);

	glutKeyboardFunc(glut_keyboard_wrap);

	GLUI *glui = GLUI_Master.create_glui("RayTracer", 0, 100 + window_width, 100);
	new GLUI_StaticText(glui, "   RayTracer Control Panel   ");
	new GLUI_Separator(glui);

	GLUI_Panel * vis_panel = new GLUI_Panel(glui, "Visual settings");

	chbShowAxes = new GLUI_Checkbox(vis_panel, "Show axes", &show_axes, 0, guit_control_wrap);
	chbShowAxes = new GLUI_Checkbox(vis_panel, "Show objects", &show_objects, 0, guit_control_wrap);

	GLUI_Panel * rays_panel = new GLUI_Panel(glui, "Configuraton of rays");
	rRays = new GLUI_RadioGroup(rays_panel, &ray_group, 0, guit_control_wrap);
	new GLUI_RadioButton(rRays, "Single ray");
	new GLUI_RadioButton(rRays, "Multiple rays");

	new GLUI_Separator(rays_panel);

	sPosx = new GLUI_Spinner(rays_panel, "Pos X:", &start_pos_x, 0, guit_control_wrap);
	sPosy = new GLUI_Spinner(rays_panel, "Pos Y:", &start_pos_y, 0, guit_control_wrap);
	sPosz = new GLUI_Spinner(rays_panel, "Pos Z:", &start_pos_z, 0, guit_control_wrap);

	new GLUI_Separator(rays_panel);

	sAlpha = new GLUI_Spinner(rays_panel, "Alpha, deg:", &alpha, 0, guit_control_wrap);
	sBeta = new GLUI_Spinner(rays_panel, "Beta, deg:", &beta, 0, guit_control_wrap);
	sGamma = new GLUI_Spinner(rays_panel, "Gamma, deg:", &gamma, 0, guit_control_wrap);

	new GLUI_Separator(rays_panel);

	sLampW = new GLUI_Spinner(rays_panel, "Width of lamp:", &lamp_width, 0, guit_control_wrap);
	sLampH = new GLUI_Spinner(rays_panel, "Height of lamp:", &lamp_height, 0, guit_control_wrap);
	sRaysW = new GLUI_Spinner(rays_panel, "Rays per width:", &rays_per_width, 0, guit_control_wrap);
	sRaysH = new GLUI_Spinner(rays_panel, "Rays per height:", &rays_per_height, 0, guit_control_wrap);

	GLUI_Panel * obj_panel = new GLUI_Panel(glui, "Object settings");
	chbAddSphere = new GLUI_Checkbox(obj_panel, "Sphere", &add_sphere, 0, guit_control_wrap);

	GLUI_Panel * sim_panel = new GLUI_Panel(glui, "Simulation");
	new GLUI_Button(sim_panel, "Step", 0, (GLUI_Update_CB)sim_step_wrap);
	sAnimDelay = new GLUI_Spinner(sim_panel, "Delay, ms:", &anim_delay, 0, guit_control_wrap);
	new GLUI_Button(sim_panel, "Start", 0, (GLUI_Update_CB)sim_start_wrap);
	new GLUI_Button(sim_panel, "Reset", 0, (GLUI_Update_CB)sim_reset_wrap);

	new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);

	glui->set_main_gfx_window(win_main);

	GLUI_Master.set_glutIdleFunc(NULL);

	sim_reset();
}

RayTracerApp::~RayTracerApp(void) {
	DEL_PTR(info);
	DEL_PTR(plane);
	DEL_PTR(sphere);

	DEL_PTR(chbShowAxes);
	DEL_PTR(chbShowObjects);
	DEL_PTR(chbAddSphere);

	DEL_PTR(rRays);

	DEL_PTR(sPosx);
	DEL_PTR(sPosy);
	DEL_PTR(sPosz);
	DEL_PTR(sAlpha);
	DEL_PTR(sBeta);
	DEL_PTR(sGamma);
	DEL_PTR(sLampW);
	DEL_PTR(sLampH);
	DEL_PTR(sRaysW);
	DEL_PTR(sRaysH);
	DEL_PTR(sAnimDelay);
}

void RayTracerApp::sim_step(void) {
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

void RayTracerApp::sim_start(void) {
	while (no_rays > 0) {
		sim_step();
		callback_glut_display();
		Sleep(anim_delay);
	}
}

void RayTracerApp::sim_reset(void) {
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
	scene.addShape(plane);
	if (add_sphere) {
		scene.addShape(sphere);
	}

	no_rays = (int)rays.size();
	steps = 0;
	total_plane_collisions = 0;
}

void RayTracerApp::callback_guit_control(int i) {
	sim_reset(); 
};

void RayTracerApp::callback_glut_display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	gluLookAt(cam_x, cam_y, cam_z, 0, 0, 0, 0, 1, 0);

	if (show_axes) {
		draw_axes(0.1f);
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

	info->Refresh();

	glutSwapBuffers();
	glutPostRedisplay();
}

void RayTracerApp::callback_glut_reshape(int w, int h) {
	if (h == 0)
		h = 1;

	float ratio = (float)w / (float)h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, w, h);

	gluPerspective(45, ratio, 1, 1000);

	glMatrixMode(GL_MODELVIEW);
}

void RayTracerApp::callback_glut_idle(void) {
	callback_glut_display();
}


void RayTracerApp::callback_glut_keyboard(unsigned char pressed_key, int x, int y) {
	switch (pressed_key) {
	case 27:
		glutLeaveMainLoop();
		break;
	case ' ':
		sim_step();
		break;
	case 's':
		sim_start();
		break;
	case 'r':
		sim_reset();
		break;
	}

	glutPostRedisplay();
}

void RayTracerApp::draw_axes(float s) {
	glPushMatrix();

	glScalef(s, s, s);

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
