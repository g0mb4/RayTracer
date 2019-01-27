#include "raytracerapp.h"

int main(int argc, char ** argv){
	RayTracerApp * app = RayTracerApp::init(&argc, argv, 800, 600);

	app->run();

	app->destroy();
	exit(EXIT_SUCCESS);
}
