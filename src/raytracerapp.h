#ifndef __RAYTRACER_APP__
#define __RAYTRACER_APP__

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <functional>
#include <list>
#include <memory>
#include <thread>

#include <sys/stat.h>
#include <time.h>

#include <windows.h>

#include <GL/glew.h>
#include <GL/GLU.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "collisionresponse.h"

#include "info.h"
#include "shape.h"

#include "camera.h"
#include "color.h"

#include "data_set.h"
#include "gnuplot.h"

#define DEL_PTR(x) \
    if (x) {       \
        delete x;  \
    };

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
    void sim_run(void);

    bool is_init_ok(void) {
        return init_ok;
    };

private:
    void draw_axes(double s);
    void render_image(void);
    double max_2(double a, double b);
    double max_3(double a, double b, double c);

    bool save_settings(const char * fname);
    bool load_settings(const char * fname);

    bool save_data(void);

    bool file_exists(const char * fname);

    bool init_ok;

    bool running = true;
    bool run_sim = false;

    int window_width, window_height;

    GLFWwindow * sim_window;
    GLFWwindow * control_window;

    const int CONTROL_WIN_WIDTH = 500;

    ImGuiWindowFlags imgui_flags;

    Info * info;

    std::vector<Ray> rays;
    ShapeSet scene;
    Plane * plane;
    Cube * cube;

    bool show_axes, show_objects, hider_cube;
    bool add_sphere, add_ellipsoid;

    int ray_group;
    int fps;
    double offset_pos_x, offset_pos_y, offset_pos_z;
    double lamp_distance;
    double alpha;

    int steps;
    int total_plane_collisions;
    double total_plane_energy;
    int no_rays;
    int direct_1st;

    double cam_x, cam_y, cam_z;

    double lamp_width, lamp_height;
    int rays_per_width, rays_per_height;

    int render_width, render_height;
    double center_y;
    double radius_x, radius_y, radius_z;

    int grid_x, grid_z;

    double refl_index_droplet, refl_index_plane, refl_index_medium;

    double energy_threshold;

    char gnuplot_path[2048];
    char settings_file[2048];

    bool autom_alpha;
    double autom_alpha_start, autom_alpha_step, autom_alpha_end;

    double prev_energy, prev_ctr;

    Data_Set data_set;
    std::vector<char *> data_set_items;
};

#endif
