# RayTracer

Simulation of a light ray passing through a water droplet.

## JS
JavaScript solution, light "as a particle".

Using [p5js](https://p5js.org) and Evan Wallace's [vector.js](https://github.com/evanw/lightgl.js).

### Usage
Run ```index.html```

## CPP - glfw/imgui
C++ solution, light "as ray", using a modified ray tracing engine made by [Hillsoft](https://github.com/Hillsoft/Raytracer-Tutorial).

Using [GLFW 3.2.1](https://www.glfw.org), [GLEW 2.1.0](http://glew.sourceforge.net) and [Dear ImGui 1.67](https://github.com/ocornut/imgui).

+ Added "Render" function
+ Not so-late-90s look

Compiled/tested on Windows (10) x64.

### Compilation
Run [premake5](https://premake.github.io):

```premake vs2017```

Open the generrated solution file from ```msvc``` then compile the project.
The executable will be in the ```bin``` directory.

## CPP - freeglut/glui (obsolete)
C++ solution, light "as ray", using a modified ray tracing engine made by [Hillsoft](https://github.com/Hillsoft/Raytracer-Tutorial).

Using [freeglut 3.0.0](http://freeglut.sourceforge.net/index.php#download) and [glui 2.36](https://sourceforge.net/projects/glui/).

Compiled/tested on Windows (10) x64.

### Compilation
Run [premake5](https://premake.github.io):

```premake vs2017```

Open the generrated solution file from ```msvc``` then compile the project.
The executable will be in the ```bin``` directory.
