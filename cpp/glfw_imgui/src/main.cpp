#include <cstdio>

#include "raytracerapp.h"

int main(int argc, char ** argv){
	RayTracerApp * app = new RayTracerApp(800, 600);

	if (app->is_init_ok()) {
		app->run();
	} else {
		fprintf(stderr, "unable to init RayTracerApp\n");
	}

	delete app;
	exit(EXIT_SUCCESS);
}
