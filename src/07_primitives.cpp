
#include "helper.h"

// TODO:
// 1. alpha 0 has no effect while drawing points
// 2. OpenGL Color Blending
// 3. glClearColor and glClear (OpenGL buffers)

// Tips:
// 1. glInterleavedArrays is useful to call glDrawElement

void check_color() {
  // check current color
  GLint color1[4];
  GLfloat color2[4];
  glGetIntegerv(GL_CURRENT_COLOR, color1);
  glGetFloatv(GL_CURRENT_COLOR, color2);
  fprintf(stdout, "GL_CURRENT_COLOR integer: %d, %d, %d, %d\n", color1[0], color1[1], color1[2], color1[3]);
  fprintf(stdout, "GL_CURRENT_COLOR float: %f, %f, %f, %f\n", color2[0], color2[1], color2[2], color2[3]);

  GLboolean rgba_mode;
  glGetBooleanv(GL_RGBA_MODE, &rgba_mode);
  fprintf(stdout, "GL_RGBA_MODE: %s\n", rgba_mode ? "yes" : "no");
}

void
init()
{
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  fprintf(stdout, "default opengl state:\n");
  check_color();
}

void
display()
{
  // Normalized Coordinates

  /////////// points
  fprintf(stdout, "draw five green points:\n");
  glColor4ub(0, 255, 0, 255);
  check_color();
  glBegin(GL_POINTS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(-0.5f, 0.5f);
  glEnd();

  fprintf(stdout, "draw four points in different colors by 1 opengl command:\n");
  GLfloat points[8] = {0.25f, 0.25f,
                       0.25f, -0.25f,
                       -0.25f, -0.25f,
                       -0.25f, 0.25f};
  GLubyte indices[4] = {0, 1, 2, 3};
  GLubyte colors[12] = {255, 0, 0,
                        0, 255, 0,
                        0, 0, 255,
                        255, 255, 0};
  glVertexPointer(2, GL_FLOAT, 0, points);
  glColorPointer(3, GL_UNSIGNED_BYTE, 0, colors);
  glDrawElements(GL_POINTS, 4, GL_UNSIGNED_BYTE, indices);

  glFlush();
}

int
main()
{
  return run(1024, 768, "Tutorial 07 - Primitives", init, display);
}
