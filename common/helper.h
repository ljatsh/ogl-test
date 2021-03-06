
#ifndef _TUTORIAL_HELPER_H_
#define _TUTORIAL_HELPER_H_

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

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

int run(int width, int height, const char* title, void (*init) (GLFWwindow* wnd), void (*display) (GLFWwindow* wnd));

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

bool load_obj(
  const char * path,
  std::vector<glm::vec3> & out_vertices, 
  std::vector<glm::vec2> & out_uvs, 
  std::vector<glm::vec3> & out_normals
);

void index_vbo(
  std::vector<glm::vec3> & in_vertices,
  std::vector<glm::vec2> & in_uvs,
  std::vector<glm::vec3> & in_normals,

  std::vector<unsigned short> & out_indices,
  std::vector<glm::vec3> & out_vertices,
  std::vector<glm::vec2> & out_uvs,
  std::vector<glm::vec3> & out_normals
);

void index_vbo_tbn(
  std::vector<glm::vec3> & in_vertices,
  std::vector<glm::vec2> & in_uvs,
  std::vector<glm::vec3> & in_normals,
  std::vector<glm::vec3> & in_tangents,
  std::vector<glm::vec3> & in_bitangents,

  std::vector<unsigned short> & out_indices,
  std::vector<glm::vec3> & out_vertices,
  std::vector<glm::vec2> & out_uvs,
  std::vector<glm::vec3> & out_normals,
  std::vector<glm::vec3> & out_tangents,
  std::vector<glm::vec3> & out_bitangents
);

void init_text_2d(const char * texturePath);
void print_text(const char * text, int x, int y, int size);
void clear_text();

void compute_tagent_basics(
  // inputs
  std::vector<glm::vec3> & vertices,
  std::vector<glm::vec2> & uvs,
  std::vector<glm::vec3> & normals,
  // outputs
  std::vector<glm::vec3> & tangents,
  std::vector<glm::vec3> & bitangents
);

#endif _TUTORIAL_HELPER_H_
