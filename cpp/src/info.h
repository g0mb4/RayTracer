#ifndef __INFO_H__
#define __INFO_H__

#include <Windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#include <stdio.h>
#include <string.h>

#include <vector>
#include <string>

#include "sysinfo.h"

enum E_PTR_TYPE {
	PT_VOID = 0,
	PT_INT,
	PT_FLOAT,
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
		_font = GLUT_BITMAP_9_BY_15;
	}

	void AddVariable(int x, int y, const char * name, void * ptr, E_PTR_TYPE tpye);

	void Write(int x, int y, const char * text);
	void Refresh(void);

private:
	int _width, _height;
	void * _font;

	void _set2D(void);
	void _set3D(void);

	void _printText(int x, int y, const char* text);

	std::vector<info_pair_t> _variables;
};


#endif

