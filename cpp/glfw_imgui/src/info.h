#ifndef __INFO_H__
#define __INFO_H__

#include <Windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <string.h>

#include <vector>
#include <string>
#include <chrono>

#include "sysinfo.h"
#include "freeglut_font.h"

#ifdef _WIN
	#define strdup	_strdup
#endif //  _WIN


enum E_PTR_TYPE {
	PT_VOID = 0,
	PT_INT,
	PT_FLOAT,
	PT_STRING,
};

typedef struct {
	int x;
	int y;
	std::string name;
	void * ptr;
	E_PTR_TYPE type;
} info_pair_t;

class Info {
public:
	Info(int width, int height) : _width(width), _height(height) {
		_font = NULL;
		_time = std::chrono::high_resolution_clock::now();
	}

	void AddVariable(int x, int y, const char * name, void * ptr);
	void AddVariable(int x, int y, const char * name, int * ptr);
	void AddVariable(int x, int y, const char * name, float * ptr);
	void AddVariable(int x, int y, const char * name, const char * ptr);

	void Write(int x, int y, const char * text);
	void Refresh(void);

private:
	void _add_variable(int x, int y, const char * name, void * ptr, E_PTR_TYPE tpye);

	int _width, _height;
	void * _font;

	void _set2D(void);
	void _set3D(void);

	void _printText(int x, int y, const char* text);

	std::vector<info_pair_t> _variables;
	std::chrono::time_point<std::chrono::steady_clock> _time;
};




#endif

