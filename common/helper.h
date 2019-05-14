
#ifndef _TUTORIAL_HELPER_H_
#define _TUTORIAL_HELPER_H_

// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

int run(int width, int height, const char* title, void (*init) (), void (*display) ());

#endif _TUTORIAL_HELPER_H_
