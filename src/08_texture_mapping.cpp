
#include "helper.h"

GLbyte texture1d[24];
GLbyte texture2d[32][32][4];

// TODO
// Mapping Algorithm

// Tips:


void
init() {
  // 1 dimension texutre
  // green red, green, red
  for (int i=0; i<=2; i+=2) {
    texture1d[4 * i]      = 0;
    texture1d[4 * i + 1]  = 255;
    texture1d[4 * i + 2]  = 0;
    texture1d[4 * i + 3]  = 255;
  }

  for (int i=1; i<=3; i+=2) {
    texture1d[4 * i]      = 255;
    texture1d[4 * i + 1]  = 0;
    texture1d[4 * i + 2]  = 0;
    texture1d[4 * i + 3]  = 255;
  }

  for (int i=2; i<=5; i+=2) {
    texture1d[4 * i]      = 0;
    texture1d[4 * i + 1]  = 255;
    texture1d[4 * i + 2]  = 255;
    texture1d[4 * i + 3]  = 255;
  }

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 6, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture1d);

  glLineWidth(10.0);

  // 2 dimention texture
  // random generated colors
  init_rand();
  int r, g, b;
  int r1=0, g1=0, b1=0;
  int row, column;
  for (int i=0; i<16; i++) {
    do {
      r = rand(0, 255);
      g = rand(0, 255);
      b = rand(0, 255);
    } while (r == r1 && g == g1 && b == b1);

    r1 = r;
    g1 = g;
    b1 = b;

    row = i / 4;
    column = (i - row * 4) * 8;
    row *= 8;

    for (int m=0; m<8; m++)
      for (int n=0; n<8; n++) {
        texture2d[row + m][column + n][0] = r;
        texture2d[row + m][column + n][1] = g;
        texture2d[row + m][column + n][2] = b;
        texture2d[row + m][column + n][3] = 255;
      }
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture2d);
}

void
display() {
  glEnable(GL_TEXTURE_1D);
  glBegin(GL_LINES);
    glTexCoord1f(0.0f);
    glVertex2f(-0.8f, -0.2f);
    glTexCoord1f(1.0f);
    glVertex2f(-0.2f, 0.8f);
  glEnd();
  glDisable(GL_TEXTURE_1D);

  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex2f(-0.3f, -0.3f);
    glTexCoord2f(1.0, 0.0); glVertex2f(0.3f, -0.3f);
    glTexCoord2f(0.0, 1.0); glVertex2f(0.3f, 0.3f);
    glTexCoord2f(1.0, 1.0); glVertex2f(-0.3f, 0.3f);
  glEnd();
  glDisable(GL_TEXTURE_2D);

  glEnable(GL_TEXTURE_2D);
  glBegin(GL_TRIANGLES);
    glTexCoord2f(0.0, 0.0); glVertex2f(0.4f, -0.3f);
    glTexCoord2f(1.0, 0.0); glVertex2f(0.9f, -0.3f);
    glTexCoord2f(0.0, 1.0); glVertex2f(0.9f, 0.3f);
  glEnd();
  glDisable(GL_TEXTURE_2D);
} 

int
main()
{
  int ret = run(1024, 768, "Tutorial 08 - Texture Mapping", init, display);

  return ret;
}
