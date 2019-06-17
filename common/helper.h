
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

#define check_gl_state() do { \
  GLenum error = glGetError(); \
  if (error != GL_NO_ERROR) { fprintf(stderr, "%s:%d open gl error %d\n", __FILE__, __LINE__, error); } \
} while (false);

int run(int width, int height, const char* title, void (*init) (), void (*display) ());

/*
 * Renders a beautiful wired teapot...
 */
void draw_wire_teapot(GLdouble size);

/*
 * Renders a beautiful filled teapot...
 */
void draw_solid_teapot(GLdouble size);

void init_rand();

/*
  Generates a random interger >= min and >= max
 */
int rand(int min, int max);

GLuint load_bmp(const char* path);

GLuint load_dds(const char* path);

void refresh_matrix_from_control(GLFWwindow* wnd);
glm::mat4 get_current_view_matrix();
glm::mat4 get_current_projection_matrix();

#endif _TUTORIAL_HELPER_H_
