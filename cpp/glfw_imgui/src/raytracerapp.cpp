#include "raytracerapp.h"

RayTracerApp::RayTracerApp(int w, int h) {
	init_ok = false;

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

	plane = new Plane(Point(0.0f, 0.0f, 0.0f), 1.0f, Vector(0.0f, 1.0f, 0.0f), Color(0.0f, 1.0f, 0.0f));
	sphere = new Sphere(Point(0.0f, 1.0f, 0.0f), 1.0f, (4.0f / 3.0f), Color(0.0f, 0.0f, 1.0f));

	show_axes = false, show_objects = true;
	add_sphere = true;

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

	render_width = 640, render_height = 480;

	if (!glfwInit()) {
		return;
	}

	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	sim_window = glfwCreateWindow(window_width, window_height, "RayTracer - Simulation", NULL, NULL);
	if (!sim_window){
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(sim_window);
	glfwSetWindowPos(sim_window, 100, 100);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// for wireframed sphere

	control_window = glfwCreateWindow(CONTROL_WIN_WIDTH, window_height, "RayTracer - Control", NULL, NULL);
	if (!control_window) {
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(control_window);
	glfwSetWindowPos(control_window, 100 + window_width, 100);

	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		return;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(control_window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGui::GetIO().IniFilename = NULL;	// no ini file;

	imgui_flags = 0;
	imgui_flags |= ImGuiWindowFlags_NoTitleBar;
	imgui_flags |= ImGuiWindowFlags_NoScrollbar;
	//imgui_flags |= ImGuiWindowFlags_MenuBar;
	imgui_flags |= ImGuiWindowFlags_NoMove;
	imgui_flags |= ImGuiWindowFlags_NoResize;
	imgui_flags |= ImGuiWindowFlags_NoCollapse;
	//imgui_flags |= ImGuiWindowFlags_NoNav;
	//imgui_flags |= ImGuiWindowFlags_NoBackground;
	//imgui_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	sim_reset();

	init_ok = true;
}

RayTracerApp::~RayTracerApp(void) {
	if(sim_window){
		glfwDestroyWindow(sim_window);
	}

	if (control_window) {
		glfwDestroyWindow(control_window);
	}
	
	glfwTerminate();

	DEL_PTR(info);
	DEL_PTR(plane);
	DEL_PTR(sphere);
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
		sim_window_draw();
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

void RayTracerApp::run(void) {
	int space_old = GLFW_RELEASE;
	int space_new = GLFW_RELEASE;

	while (running && !glfwWindowShouldClose(sim_window)){
		glfwPollEvents();

		if (glfwGetKey(sim_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			running = false;
		}

		space_new = glfwGetKey(sim_window, GLFW_KEY_SPACE);
		if (space_new == GLFW_PRESS && space_old == GLFW_RELEASE) {
			sim_step();
		}
		space_old = space_new;

		if (glfwGetKey(sim_window, GLFW_KEY_S) == GLFW_PRESS) {
			sim_start();
		}

		if (glfwGetKey(sim_window, GLFW_KEY_R) == GLFW_PRESS) {
			sim_reset();
		}

		control_window_draw();

		sim_window_draw();
	};
}

void RayTracerApp::sim_window_draw(void) {
	glfwMakeContextCurrent(sim_window);

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

	glfwSwapBuffers(sim_window);
}

void RayTracerApp::control_window_draw(void) {
	glfwMakeContextCurrent(control_window);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

	if (ImGui::Begin("Control", NULL, imgui_flags)) {
		if (ImGui::CollapsingHeader("Configuration of rays")) {
			if (ImGui::RadioButton("Single ray", &ray_group, 0)) { sim_reset(); }
			if (ImGui::RadioButton("Multiple rays", &ray_group, 1)) { sim_reset(); }

			ImGui::Separator();
			ImGui::PushItemWidth(150);
			if (ImGui::InputFloat("Pos X", &start_pos_x, 0.01f, 1.0f, "%.3f")) { sim_reset(); }
			if (ImGui::InputFloat("Pos Y", &start_pos_y, 0.01f, 1.0f, "%.3f")) { sim_reset(); }
			if (ImGui::InputFloat("Pos Z", &start_pos_z, 0.01f, 1.0f, "%.3f")) { sim_reset(); }

			ImGui::Separator();

			if(ImGui::InputFloat("Alpha, deg", &alpha, 0.01f, 1.0f, "%.3f")){ sim_reset(); }
			if(ImGui::InputFloat("Beta, deg",  &beta,  0.01f, 1.0f, "%.3f")){ sim_reset(); }
			if(ImGui::InputFloat("Gamma, deg", &gamma, 0.01f, 1.0f, "%.3f")){ sim_reset(); }

			if (ray_group == 1) {
				ImGui::Separator();
				if (ImGui::InputFloat("Width of lamp", &lamp_width, 0.01f, 1.0f, "%.3f")) { sim_reset(); }
				if (ImGui::InputFloat("Height of lamp", &lamp_width, 0.01f, 1.0f, "%.3f")) { sim_reset(); }
				if (ImGui::InputInt("Rays per width", &rays_per_width)) { sim_reset(); }
				if (ImGui::InputInt("Rays per height", &rays_per_height)) { sim_reset(); }
			}
		}

		if (ImGui::CollapsingHeader("Object settings")) {
			if(ImGui::Checkbox("Spheres", &add_sphere)) { sim_reset(); }
		}

		if (ImGui::CollapsingHeader("Visual")) {
			ImGui::Checkbox("Show axes", &show_axes);
			ImGui::Checkbox("Show objects", &show_objects);
		}

		if (ImGui::CollapsingHeader("Render")) {
			ImGui::InputInt("Width", &render_width);
			ImGui::InputInt("Height", &render_height);
			if (ImGui::Button("Render Image")) {
				render_image();
			}
		}
		
		ImGui::Separator();
		ImGui::Separator();

		ImGui::BeginGroup();
		ImGui::Text("--- SIMULATION");
		ImGui::InputInt("Delay, ms", &anim_delay);

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(186, 154, 0));
		if (ImGui::Button("Step")) {
			sim_step();
		}
		ImGui::PopStyleColor(1);
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(16, 163, 0));
		if (ImGui::Button("Start")) {
			sim_start();
		}
		ImGui::PopStyleColor(1);
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(247, 12, 12));
		if (ImGui::Button("Reset")) {
			sim_reset();
		}
		ImGui::PopStyleColor(1);
		ImGui::EndGroup();

		ImGui::Separator();
		ImGui::Separator();
		if (ImGui::Button("Quit")) {
			running = false;
		}

		ImGui::End();
	}

	ImGui::Render();

	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(control_window);
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

void RayTracerApp::render_image(void) {
	char title[256];
	sprintf_s(title, 256, "RayTracer - Rendered Image - %dx%d - rendering ...", render_width, render_height);

	GLFWwindow * render_window = glfwCreateWindow(render_width, render_height, title, NULL, NULL);
	if (!render_window) {
		return;
	}

	glfwMakeContextCurrent(render_window);
	glfwSetWindowPos(render_window, 100, 100);

	std::vector<Color> image;

	auto cosd = [](float a) {
		return cos(a * (PI / 180.0f));
	};

	PerspectiveCamera camera(Point(start_pos_x, start_pos_y, start_pos_z), Vector(cosd(alpha), cosd(beta), cosd(gamma)), Vector(), 25.0f * PI / 180.0f, (float)render_width / (float)render_height);
	Color * buf = new Color[render_width * render_height];

	auto t1 = std::chrono::high_resolution_clock::now();
	for (int x = 0; x < render_width; x++) {
		for (int y = 0; y < render_height; y++) {
			Vector2 screenCoord((2.0f * x) / render_width - 1.0f, (-2.0f * y) / render_height + 1.0f);
			Ray ray = camera.makeRay(screenCoord);
			Intersection intersection(ray);
			Color c = Color(0.0f);

			if (scene.intersect(intersection)) {
				c = intersection.pShape->color;
			}

			buf[y * render_width + x] = c;
		}
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	auto diff = t2 - t1;
	auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();

	sprintf_s(title, 256, "RayTracer - Rendered Image - %dx%d - %llu ms", render_width, render_height, duration_ms);
	glfwSetWindowTitle(render_window, title);

	while (!glfwWindowShouldClose(render_window)) {
		glfwPollEvents();

		glfwMakeContextCurrent(render_window);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glLoadIdentity();
		gluOrtho2D(0, render_width, render_height, 0);

		glBegin(GL_POINTS);
		for (int x = 0; x < render_width; x++) {
			for (int y = 0; y < render_height; y++) {
				Color * c = &buf[y * render_width + x];
				glColor3f(c->r, c->g, c->b);
				glVertex2i(x, y);
				
			}
		}
		glEnd();
		glfwSwapBuffers(render_window);
	}

	delete[] buf;

	if (render_window) {
		glfwDestroyWindow(render_window);
	}
}
