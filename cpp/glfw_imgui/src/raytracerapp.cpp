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
	info->AddVariable(10, 72, "PLANE ENERGY", &total_plane_energy);
	info->AddVariable(10, 84, "NUMBER OF RAYS", &no_rays);

	info->AddVariable(10, 108, "esc", "stop sim");
	info->AddVariable(10, 120, "spcae", "step sim");
	info->AddVariable(10, 132, "s", "start sim");
	info->AddVariable(10, 144, "r", "reset sim");
	info->AddVariable(10, 168, "SIM RUNNING", &run_sim);

	info->AddVariable(10, 192, "alpha", &alpha);

	info->AddVariable(10, 204, "1", "side cam");
	info->AddVariable(10, 216, "2", "top cam");

	plane = nullptr;
	cube = new Cube(Point(0.0f, -1.0f, 0.0f), 2.0f, Color(0.0f, 0.0f, 0.0f));

	show_axes = false, show_objects = true;
	hider_cube = true;
	add_sphere = true;
	add_ellipsoid = true;

	ray_group = 0;
	start_pos_x = -2.0f, start_pos_y = 0.5f, start_pos_z = 0.0f;
	alpha = 0;

	steps = 0;
	total_plane_collisions = 0;
	total_plane_energy = 0;
	no_rays = 0;

	cam_x = 0, cam_y = 0, cam_z = 8;

	lamp_width = 1.9f, lamp_height = 2.0f;
	rays_per_width = 10, rays_per_height = 10;

	center_y = 0.0f;
	radius_x = radius_y = radius_z = 1.0f;

	render_width = 640, render_height = 480;

	energy_threshold = 0.01;

	refl_index_droplet = 4.0f / 3.0f;
	refl_index_plane = 4.0f / 3.0f;
	refl_index_medium = 1.0f;

	grid_x = 1;
	grid_z = 1;

	strncpy(gnuplot_path, "c:\\Program Files\\gnuplot\\bin\\gnuplot.exe", 2048);
	strncpy(settings_file, "default", 2048);

	autom_alpha = false;
	autom_alpha_start = 0;	
	autom_alpha_step = 0.01;
	autom_alpha_end = 20;

	data_set.create("alpha");
	data_set.create("plane_energy");
	data_set.create("plane_collisions");

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

	load_settings("default");

	sim_reset();

	run_sim = false;

	init_ok = true;

	data_set_items = data_set.get_names_c();
}

RayTracerApp::~RayTracerApp(void) {
	if(sim_window){
		glfwDestroyWindow(sim_window);
	}

	if (control_window) {
		glfwDestroyWindow(control_window);
	}

	for (int n = 0; n < data_set_items.size(); n++) {
		if (data_set_items[n]) {
			free(data_set_items[n]);
		}
	}
	
	glfwTerminate();

	DEL_PTR(info);
	DEL_PTR(plane);
	DEL_PTR(cube);
}

void RayTracerApp::sim_step(void) {
	std::vector<Ray> new_set;

	for (auto ray : rays) {
		if (ray.valid) {
			Intersection intersection(ray);
			if (scene.intersect(intersection)) {
				Ray reflected;
				Ray refracted;

				CollisionResponse c(intersection, &ray, refl_index_medium, &scene, &reflected, &refracted);

				printf("  reflected: %f %s (%s) dir: (%f; %f; %f)\n", reflected.energy, reflected.energy == 0.0 ? "(ZERO)" : "", reflected.valid ? "vaild" : "invalid", reflected.direction.x, reflected.direction.y, reflected.direction.z);
				printf("  refracted: %f (%s) dir: (%f; %f; %f)\n", refracted.energy, refracted.valid ? "vaild" : "invalid", refracted.direction.x, refracted.direction.y, refracted.direction.z);

				if (intersection.pShape->type == T_PLANE) {
					total_plane_collisions++;

					if (reflected.valid && reflected.energy > energy_threshold) {
						new_set.push_back(reflected);
					}

					if (refracted.valid) {
						total_plane_energy += refracted.energy;
					}

				} else {
					if (reflected.valid && reflected.energy > energy_threshold) {
						new_set.push_back(reflected);
					}

					if (refracted.valid && refracted.energy > energy_threshold) {
						new_set.push_back(refracted);
					}
				}
			}
		}
	}

//	if (rays != new_set) {
		rays = new_set;
		printf("new set: %u\n", new_set.size());
//	} else {
//		rays.clear(); // end of sim
//	}
	
	steps++;
}

void RayTracerApp::sim_run(void) {
	data_set.clear_data();

	for (alpha = autom_alpha ? autom_alpha_start : alpha;
		 !autom_alpha || alpha <= autom_alpha_end;
		 alpha += autom_alpha ? autom_alpha_step : 0.0f) {

		sim_reset();
		while (run_sim && rays.size() > 0) {
			sim_step();
			sim_window_draw();
		}

		data_set.add_data("alpha", alpha);
		data_set.add_data("plane_energy", total_plane_energy);
		data_set.add_data("plane_collisions", total_plane_collisions);

		if (!autom_alpha) {
			break;
		}
	}

	run_sim = false;
}

void RayTracerApp::sim_start(void) {
	run_sim = true;
	std::thread t(&RayTracerApp::sim_run, this);
	t.detach();
}

double RayTracerApp::max_2(double a, double b) {
	return a > b ? a : b;
}

double RayTracerApp::max_3(double n1, double n2, double n3) {
	if (n1 > n2 && n1 > n3){
		return n1;
	} else if (n2 > n1 && n2 > n3){
		return n2;
	} else {
		return n3;
	}
}

void RayTracerApp::sim_reset(void) {
	auto cosd = [](double a) {
		return cos(a * (PI / 180.0f));
	};

	auto sind = [](double a) {
		return sin(a * (PI / 180.0f));
	};

	rays.clear();

	// single ray
	if (ray_group == 0) {
		rays.push_back(Ray(Point(start_pos_x, start_pos_y, start_pos_z), Vector(cosd(alpha), sind(alpha), 0)));
	}
	else {
		double d_y = lamp_width / (double)rays_per_width;
		double d_z = lamp_height / (double)rays_per_height;
		double start_y = start_pos_y - (lamp_width / 2.0f);
		double end_y = start_pos_y + (lamp_width / 2.0f);
		double start_z = start_pos_z - (lamp_height / 2.0f);
		double end_z = start_pos_z + (lamp_height / 2.0f);

		for (double y = end_y; y > start_y; y -= d_y) {
			if (y <= 0.0f) {
				break;
			}
			for (double z = start_z; z < end_z; z += d_z) {
				rays.push_back(Ray(Point(start_pos_x, y, z), Vector(cosd(alpha), sind(alpha), 0)));
			}
		}
	}

	scene.clear();

	if (plane != nullptr) {
		delete plane;
	}
	plane = new Plane(Point(0.0f, 0.001f, 0.0f), 1.0f, Vector(0.0f, 1.0f, 0.0f), refl_index_plane, Color(0.0f, 1.0f, 0.0f));

	scene.addShape(plane);
	if (add_ellipsoid) {
		for (int i = 0; i < grid_x; i++) {
			for (int j = 0; j < grid_z; j++) {
				double x = (i - (grid_x / 2)) * 2;
				double z = (j - (grid_z / 2)) * 2;

				scene.addShape(new Ellipsoid(Point(x, center_y, z), Point(radius_x, radius_y, radius_z), refl_index_droplet, Color(0.0f, 0.0f, 1.0f)));
			}
		}
	}

	double hsl = max_3(radius_x, radius_y, radius_z) * max_2(grid_x, grid_z) * 2;

	cube->halfSideLength = hsl;
	cube->centre.y = -hsl;

	scene.addShape(cube);
	
	no_rays = (int)rays.size();
	steps = 0;
	total_plane_collisions = 0;
	total_plane_energy = 0;
}

void RayTracerApp::run(void) {
	int space_old = GLFW_RELEASE;
	int space_new = GLFW_RELEASE;

	while (running && !glfwWindowShouldClose(sim_window)){
		glfwPollEvents();

		if (glfwGetKey(sim_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			run_sim = false;
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

		if (glfwGetKey(sim_window, GLFW_KEY_1) == GLFW_PRESS) {
			cam_x = 0, cam_y = 0, cam_z = 8;
		}

		if (glfwGetKey(sim_window, GLFW_KEY_2) == GLFW_PRESS) {
			cam_x = 0, cam_y = 8, cam_z = 0.0000000001;
		}

		control_window_draw();

		sim_window_draw();
	};
}

void RayTracerApp::sim_window_draw(void) {
	glfwMakeContextCurrent(sim_window);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (!run_sim) {
		gluLookAt(cam_x, cam_y, cam_z, 0, 0, 0, 0, 1, 0);

		if (show_axes) {
			draw_axes(0.1f);
		}

		if (show_objects) {
			for (auto s : scene.shapes) {
				if (s->type == T_CUBE) {
					if (hider_cube) {
						s->draw();
					}
				}
				else {
					s->draw();
				}
			}
		}
	
		no_rays = 0;
		for (auto ray : rays) {
			if (ray.valid) {
				ray.draw();
				no_rays++;
			}
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
		if (ImGui::CollapsingHeader("Rays")) {
			if (ImGui::RadioButton("Single ray", &ray_group, 0)) { sim_reset(); }
			if (ImGui::RadioButton("Multiple rays", &ray_group, 1)) { sim_reset(); }

			ImGui::Separator();
			ImGui::PushItemWidth(150);
			if (ImGui::InputDouble("Pos X", &start_pos_x, 0.01f, 1.0f, "%.4f")) { sim_reset(); }
			if (ImGui::InputDouble("Pos Y", &start_pos_y, 0.01f, 1.0f, "%.4f")) { sim_reset(); }
			if (ImGui::InputDouble("Pos Z", &start_pos_z, 0.01f, 1.0f, "%.4f")) { sim_reset(); }

			ImGui::Separator();

			if(ImGui::InputDouble("Alpha, deg", &alpha, 0.01f, 1.0f, "%.4f")){ sim_reset(); }

			if (ray_group == 1) {
				ImGui::Separator();
				if (ImGui::InputDouble("Width of lamp", &lamp_width, 0.01f, 1.0f, "%.4f")) { sim_reset(); }
				if (ImGui::InputDouble("Height of lamp", &lamp_height, 0.01f, 1.0f, "%.4f")) { sim_reset(); }
				if (ImGui::InputInt("Rays per width", &rays_per_width)) { sim_reset(); }
				if (ImGui::InputInt("Rays per height", &rays_per_height)) { sim_reset(); }
			}
		}

		if (ImGui::CollapsingHeader("Objects")) {
			if(ImGui::Checkbox("Ellipsoid", &add_ellipsoid)) { sim_reset(); }
			ImGui::Separator();
			if (ImGui::InputDouble("Center Y", &center_y, 0.01f, 0.1f, "%.4f")) { sim_reset(); }
			if (ImGui::InputDouble("Radius X", &radius_x, 0.01f, 0.1f, "%.4f")) { sim_reset(); }
			if (ImGui::InputDouble("Radius Y", &radius_y, 0.01f, 0.1f, "%.4f")) { sim_reset(); }
			if (ImGui::InputDouble("Radius Z", &radius_z, 0.01f, 0.1f, "%.4f")) { sim_reset(); }
			if (ImGui::InputInt("Objects per X", &grid_x)) { sim_reset(); }
			if (ImGui::InputInt("Objects per Z", &grid_z)) { sim_reset(); }
		}

		if (ImGui::CollapsingHeader("Properties")) {
			if (ImGui::InputDouble("Refl. index of droplet", &refl_index_droplet, 0.01f, 0.1f, "%.4f")) { sim_reset(); }
			if (ImGui::InputDouble("Refl. index of plane", &refl_index_plane, 0.01f, 0.1f, "%.4f")) { sim_reset(); }
			if (ImGui::InputDouble("Refl. index of medium", &refl_index_medium, 0.01f, 0.1f, "%.4f")) { sim_reset(); }
			if (ImGui::InputDouble("Energy threshold", &energy_threshold, 0.01f, 0.1f, "%.4f")) { sim_reset(); }
		}

		if (ImGui::CollapsingHeader("View")) {
			ImGui::Checkbox("Show axes (red: X, green: Y, blue: Z)", &show_axes);
			ImGui::Checkbox("Show objects", &show_objects);
			ImGui::Checkbox("Hider cube", &hider_cube);
			ImGui::InputDouble("Cam X", &cam_x, 0.01f, 0.1f, "%.4f");
			ImGui::InputDouble("Cam Y", &cam_y, 0.01f, 0.1f, "%.4f");
			ImGui::InputDouble("Cam Z", &cam_z, 0.01f, 0.1f, "%.4f");
		}

		if (ImGui::CollapsingHeader("gnuplot")) {
			ImGui::InputText("path of gnuplot", gnuplot_path, 2048);
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
		if (run_sim) {
			ImGui::Text("--- SIMULATION : RUNNING");
		} else {
			ImGui::Text("--- SIMULATION");
		}

		if (ImGui::CollapsingHeader("Automation")) {
			ImGui::Checkbox("Automate alpha", &autom_alpha);
			if (autom_alpha) {
				ImGui::InputDouble("Start alpha", &autom_alpha_start, 0.01f, 0.1f, "%.4f");
				ImGui::InputDouble("Step alpha" , &autom_alpha_step, 0.01f, 0.1f, "%.4f");
				ImGui::InputDouble("End alpha"  , &autom_alpha_end, 0.01f, 0.1f, "%.4f");
			}
		}

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(186, 154, 0));
		if (ImGui::Button("Step")) {
			sim_step();
		}
		ImGui::PopStyleColor(1);
		ImGui::SameLine();

		if (run_sim) {
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(247, 12, 12));
			if (ImGui::Button("Stop")) {
				run_sim = false;
			}
		} else {
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(16, 163, 0));
			if (ImGui::Button("Start")) {
				sim_start();
			}
		}
		ImGui::PopStyleColor(1);
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(66, 75, 244));
		if (ImGui::Button("Reset")) {
			sim_reset();
		}
		ImGui::PopStyleColor(1);
		ImGui::EndGroup();

		if (ImGui::CollapsingHeader("Data")) {
			static char * item_x = data_set_items[0];
			if (ImGui::BeginCombo("X", item_x)) // The second parameter is the label previewed before opening the combo.
			{
				for (int n = 0; n < data_set_items.size(); n++)
				{
					bool is_selected = (item_x == data_set_items[n]);
					if (ImGui::Selectable(data_set_items[n], is_selected))
						item_x = data_set_items[n];
					if (is_selected)
						ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
				}
				ImGui::EndCombo();
			}

			static char * item_y = data_set_items[0];
			if (ImGui::BeginCombo("Y", item_y)) // The second parameter is the label previewed before opening the combo.
			{
				for (int n = 0; n < data_set_items.size(); n++)
				{
					bool is_selected = (item_y == data_set_items[n]);
					if (ImGui::Selectable(data_set_items[n], is_selected))
						item_y = data_set_items[n];
					if (is_selected)
						ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("plot 2D")) {
				Gnuplot gp(gnuplot_path);

				gp.plot(data_set.get_data(item_x), data_set.get_data(item_y));
			}
		}

		ImGui::Separator();
		ImGui::Separator();
		if (ImGui::CollapsingHeader("Save/Load")) {

			ImGui::InputText("settings file", settings_file, 2048);
			if (ImGui::Button("Save")) {
				save_settings(settings_file);
				sim_reset();
			}

			if (ImGui::Button("Load")) {
				load_settings(settings_file);
				sim_reset();
			}
		}

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

void RayTracerApp::draw_axes(double s) {
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
	glfwSetWindowTitle(render_window, title);

	std::vector<Color> image;

	auto cosd = [](double a) {
		return cos(a * (PI / 180.0f));
	};

	auto sind = [](double a) {
		return sin(a * (PI / 180.0f));
	};

	PerspectiveCamera camera(Point(start_pos_x, start_pos_y, start_pos_z), Vector(cosd(alpha), sind(alpha), 0), Vector(), 25.0f * PI / 180.0f, (double)render_width / (double)render_height);
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

bool RayTracerApp::save_settings(const char * fname) {
	char file[2048];
	snprintf(file, 2048, "%s.rts", fname);

	FILE * fp = fopen(file, "wb");
	
	if (!fp){
		fprintf(stderr, "unable to create '%s'\n", file);
		return false;
	}

	fwrite(&show_axes, sizeof(show_axes), 1, fp);
	fwrite(&show_objects, sizeof(show_objects), 1, fp);
	fwrite(&hider_cube, sizeof(hider_cube), 1, fp);

	fwrite(&add_sphere, sizeof(add_sphere), 1, fp);
	fwrite(&add_ellipsoid, sizeof(add_ellipsoid), 1, fp);

	fwrite(&ray_group, sizeof(ray_group), 1, fp);

	fwrite(&start_pos_x, sizeof(start_pos_x), 1, fp);
	fwrite(&start_pos_y, sizeof(start_pos_y), 1, fp);
	fwrite(&start_pos_z, sizeof(start_pos_z), 1, fp);

	fwrite(&cam_x, sizeof(cam_x), 1, fp);
	fwrite(&cam_y, sizeof(cam_y), 1, fp);
	fwrite(&cam_z, sizeof(cam_z), 1, fp);

	fwrite(&lamp_width, sizeof(lamp_width), 1, fp);
	fwrite(&lamp_height, sizeof(lamp_height), 1, fp);
	fwrite(&rays_per_width, sizeof(rays_per_width), 1, fp);
	fwrite(&rays_per_height, sizeof(rays_per_height), 1, fp);

	fwrite(&render_width, sizeof(render_width), 1, fp);
	fwrite(&render_height, sizeof(render_height), 1, fp);

	fwrite(&center_y, sizeof(center_y), 1, fp);
	fwrite(&radius_x, sizeof(radius_x), 1, fp);
	fwrite(&radius_y, sizeof(radius_y), 1, fp);
	fwrite(&radius_z, sizeof(radius_z), 1, fp);

	fwrite(&grid_x, sizeof(grid_x), 1, fp);
	fwrite(&grid_z, sizeof(grid_z), 1, fp);

	fwrite(&refl_index_droplet, sizeof(refl_index_droplet), 1, fp);
	fwrite(&refl_index_plane, sizeof(refl_index_plane), 1, fp);
	fwrite(&refl_index_medium, sizeof(refl_index_medium), 1, fp);
	fwrite(&energy_threshold, sizeof(energy_threshold), 1, fp);

	fwrite(&autom_alpha, sizeof(autom_alpha), 1, fp);
	fwrite(&autom_alpha_start, sizeof(autom_alpha_start), 1, fp);
	fwrite(&autom_alpha_step, sizeof(autom_alpha_step), 1, fp);
	fwrite(&autom_alpha_end, sizeof(autom_alpha_end), 1, fp);

	fclose(fp);

	printf("'%s' was saved\n", file);

	return true;
}

bool RayTracerApp::load_settings(const char * fname) {
	char file[2048];
	snprintf(file, 2048, "%s.rts", fname);

	FILE * fp = fopen(file, "rb");

	if (!fp) {
		fprintf(stderr, "unable to read '%s'\n", file);
		return false;
	}

	fread(&show_axes, sizeof(show_axes), 1, fp);
	fread(&show_objects, sizeof(show_objects), 1, fp);
	fread(&hider_cube, sizeof(hider_cube), 1, fp);

	fread(&add_sphere, sizeof(add_sphere), 1, fp);
	fread(&add_ellipsoid, sizeof(add_ellipsoid), 1, fp);

	fread(&ray_group, sizeof(ray_group), 1, fp);

	fread(&start_pos_x, sizeof(start_pos_x), 1, fp);
	fread(&start_pos_y, sizeof(start_pos_y), 1, fp);
	fread(&start_pos_z, sizeof(start_pos_z), 1, fp);

	fread(&cam_x, sizeof(cam_x), 1, fp);
	fread(&cam_y, sizeof(cam_y), 1, fp);
	fread(&cam_z, sizeof(cam_z), 1, fp);

	fread(&lamp_width, sizeof(lamp_width), 1, fp);
	fread(&lamp_height, sizeof(lamp_height), 1, fp);
	fread(&rays_per_width, sizeof(rays_per_width), 1, fp);
	fread(&rays_per_height, sizeof(rays_per_height), 1, fp);

	fread(&render_width, sizeof(render_width), 1, fp);
	fread(&render_height, sizeof(render_height), 1, fp);

	fread(&center_y, sizeof(center_y), 1, fp);
	fread(&radius_x, sizeof(radius_x), 1, fp);
	fread(&radius_y, sizeof(radius_y), 1, fp);
	fread(&radius_z, sizeof(radius_z), 1, fp);

	fread(&grid_x, sizeof(grid_x), 1, fp);
	fread(&grid_z, sizeof(grid_z), 1, fp);

	fread(&refl_index_droplet, sizeof(refl_index_droplet), 1, fp);
	fread(&refl_index_plane, sizeof(refl_index_plane), 1, fp);
	fread(&refl_index_medium, sizeof(refl_index_medium), 1, fp);
	fread(&energy_threshold, sizeof(energy_threshold), 1, fp);

	fread(&autom_alpha, sizeof(autom_alpha), 1, fp);
	fread(&autom_alpha_start, sizeof(autom_alpha_start), 1, fp);
	fread(&autom_alpha_step, sizeof(autom_alpha_step), 1, fp);
	fread(&autom_alpha_end, sizeof(autom_alpha_end), 1, fp);

	fclose(fp);

	printf("'%s' was loaded\n", file);

	return true;
}

