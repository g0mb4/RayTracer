#ifndef __GLUT_FONT_H__
#define __GLUT_FONT_H__

#include <GLFW/glfw3.h>

// fg_internal.h 
// line: 748
typedef struct tagSFG_FontMod SFG_FontMod;
struct tagSFG_FontMod
{
	int  Quantity;     /* Number of chars in font          */
	int  Height;       /* Height of the characters         */
	const unsigned char** Characters;   /* The characters mapping           */

	float xorig, yorig; /* Relative origin of the character */
};

// fg_font.c
// line: 99
void glutBitmapCharacterMod(int character);

// fg_font.c
// line: 133
void glutBitmapStringMod(const unsigned char * string);

#endif
