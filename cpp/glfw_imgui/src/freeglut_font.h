#ifndef __GLUT_FONT_H__
#define __GLUT_FONT_H__

#include <GLFW/glfw3.h>

// fg_internal.h 
// line: 748
typedef struct tagSFG_Font SFG_Font;
struct tagSFG_Font
{
	int  Quantity;     /* Number of chars in font          */
	int  Height;       /* Height of the characters         */
	const unsigned char** Characters;   /* The characters mapping           */

	float xorig, yorig; /* Relative origin of the character */
};

// fg_font.c
// line: 99
void glutBitmapCharacter(int character);

// fg_font.c
// line: 133
void glutBitmapString(const unsigned char * string);

#endif
