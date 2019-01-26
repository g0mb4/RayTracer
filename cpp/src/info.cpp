#include "info.h"

void Info::Refresh(void) {
	_set2D();

	glColor3f(1.0f, 1.0f, 1.0f);

	char buf[256];
	for (info_pair_t i : _variables) {
		memset(buf, 0, 256);

		if (i.name == "MEM") {
			si_getUsedMemory(buf, 256);
		}
		else {
			switch (i.type) {
			case PT_INT:
				sprintf_s(buf, 256, "%s: %d", i.name.c_str(), *(int*)i.ptr);
				break;

			case PT_FLOAT:
				sprintf_s(buf, 256, "%s: %.2f", i.name.c_str(), *(float*)i.ptr);
				break;
			}
		}

		_printText(i.x, i.y, buf);
	}
	
	_set3D();
}


void Info::AddVariable(int x, int y, const char * name, void * ptr, E_PTR_TYPE type) {
	info_pair_t info;
	memset(&info, 0, sizeof(info));

	info.x = x;
	info.y = y;
	info.name = std::string(name);
	info.ptr = ptr;
	info.type = type;

	_variables.push_back(info);
}

void Info::Write(int x, int y, const char * text) {
	_printText(x, y, text);
}

void Info::_printText(int x, int y, const char* text) {
	glRasterPos2i(x, y);

	for (int i = 0; i < strlen(text); i++) {
		glutBitmapCharacter(_font, text[i]);
	}
}

void Info::_set2D(void) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, _width, _height, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Info::_set3D(void) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)_width / (float)_height, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

