# RayTracer

Light in a water drop.

## JS
JavaScript solution, light "as a particle". 

### Usage
Run ```index.html```

## CPP
C++ solution, using a modified ray tracing engine made by [Hillsoft](https://github.com/Hillsoft/Raytracer-Tutorial).

Using [freeglut 3.0.0.](http://freeglut.sourceforge.net/index.php#download) and [glui 2.36](https://sourceforge.net/projects/glui/).

Currently working on Windows (10) x64.

### Compilation
Copy ```freeglut``` binaries (bin, include, lib)  into ```lib_3rd/freeglut-3.0.0``` .

Extract the glui archive into ```lib_3rd``` then compile glui: https://masdel.wordpress.com/2010/06/13/installing-glui-using-vc-on-windows/.

```lib_3rd``` should be:
```
├───freeglut-3.0.0
│   ├───bin
│   │   └───x64
│   ├───include
│   │   └───GL
│   └───lib
│       └───x64
└───glui-2.36
    ├───src
    │   ├───devcpp
    │   ├───doc
    │   ├───example
    │   ├───include
    │   │   └───GL
    │   ├───msvc
    │   │   ├───Backup
    │   │   ├───bin
    │   │   ├───Debug
    │   │   ├───DebugDLL
    │   │   ├───lib
    │   │   └───x64
    │   │       ├───Debug
    │   │       │   └───_glui library.tlog
    │   │       └───Debug GLUIDLL
    │   │           ├───example1.tlog
    │   │           ├───example2.tlog
    │   │           ├───example3.tlog
    │   │           ├───example4.tlog
    │   │           ├───example5.tlog
    │   │           ├───example6.tlog
    │   │           └───_gluidll library.tlog
    │   └───tools
	└───www
```

Run ```premake5```:

```premake vs2017```

Open the generrated solution file from ```msvc``` then compile the project.
The executable will be in the ```bin``` directory.
