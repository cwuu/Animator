#ifndef TEXTURE_H
#define TEXTURE_H

// reference code: https://github.com/vivek2612/Human-Model-OpenGL/blob/master/src/tex.h

#include <FL/gl.h>
#include <FL/glut.H>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;

class Texture {
public:
	GLuint loadBMP_custom(const char * imagepath);
};

#endif


