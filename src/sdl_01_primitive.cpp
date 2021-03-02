
#include <stdio.h>

#include "SDL.h"
#include "SDL_opengl.h"

void show_gl_info() {
  fprintf(stdout, "gl information:\n");
  fprintf(stdout, "vendor:%s\n", glGetString(GL_VENDOR));
  fprintf(stdout, "renderer:%s\n", glGetString(GL_RENDERER));
  fprintf(stdout, "version:%s\n", glGetString(GL_VERSION));
  fprintf(stdout, "glsl version:%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
  //fprintf(stdout, "extensions:%s\n", glGetString(GL_EXTENSIONS));

  GLboolean enabled = glIsEnabled(GL_VERTEX_ARRAY);
  fprintf(stdout, "GL_VERTEX_ARRAY enabled: %s\n", enabled ? "true" : "false");
}

#ifdef WIN32
#undef main
#endif // WIN32
int main(int argc, const char* argv[])
{
  SDL_Window *window;                    // Declare a pointer

  SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2

  // Create an application window with the following settings:
  window = SDL_CreateWindow(
      "An SDL2 window",                  // window title
      SDL_WINDOWPOS_UNDEFINED,           // initial x position
      SDL_WINDOWPOS_UNDEFINED,           // initial y position
      1024,                              // width, in pixels
      768,                               // height, in pixels
      SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE                  // flags - see below
  );

  // Check that the window was successfully created
  if (window == NULL) {
      // In the case that the window could not be made...
      printf("Could not create window: %s\n", SDL_GetError());
      return 1;
  }

  // Create an OpenGL context associated with the window.
  SDL_GLContext glcontext = SDL_GL_CreateContext(window);
  show_gl_info();

  // http://www.sdltutorials.com/sdl-opengl-tutorial-basics
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,          8);

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,          16);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,            32);

  SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,        8);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,    8);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,        8);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,    8);

  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);

  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  2);

  // if((Surf_Display = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL)) == NULL) {
  //     return false;
  // }

  glClearColor(0, 0, 0, 0);

  glViewport(0, 0, 1024, 768);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0, 1024, 768, 0, 1, -1);

  glMatrixMode(GL_MODELVIEW);

  glEnable(GL_TEXTURE_2D);

  glLoadIdentity();

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  SDL_Event event;

  glEnableClientState(GL_VERTEX_ARRAY);

  do{
    // // Clear the screen
    // glClear(GL_COLOR_BUFFER_BIT);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Point
    glBegin(GL_POINTS);
      glVertex2i(10, 10);
      glVertex2i(20, 20);
    glEnd();

    // Lines
    glBegin(GL_LINES);
      glVertex2i(60, 5);
      glVertex2i(65, 10);

      glVertex2i(65, 15);
      glVertex2i(70, 15);

      glVertex2i(70, 20);
      glVertex2i(80, 30);
    glEnd();

    glBegin(GL_LINE_STRIP);
      glVertex2i(60+50, 5);
      glVertex2i(65+50, 10);
      glVertex2i(65+50, 15);
      glVertex2i(70+50, 15);
      glVertex2i(70+50, 20);
      glVertex2i(80+50, 30);
    glEnd();

    glBegin(GL_LINE_LOOP);
      glVertex2i(60+100, 5);
      glVertex2i(65+100, 10);
      glVertex2i(65+100, 15);
      glVertex2i(70+100, 15);
      glVertex2i(70+100, 20);
      glVertex2i(80+100, 30);
    glEnd();

    ///////////////// Fill Area

    glBegin(GL_TRIANGLES);
      glVertex2i(225, 5);
      glVertex2i(200, 25);
      glVertex2i(240, 25);

      glVertex2i(200, 30);
      glVertex2i(240, 30);
      glVertex2i(225, 59);
    glEnd();

    glBegin(GL_TRIANGLE_STRIP);
      glVertex2i(225+50, 5);
      glVertex2i(200+50, 25);
      glVertex2i(240+50, 25);
      glVertex2i(225+50, 59);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
      glVertex2i(325, 0);
      glVertex2i(300, 50);
      glVertex2i(310, 50);
      glVertex2i(330, 25);
      glVertex2i(350, 25);
    glEnd();

    // 定点需要逆时针设置， 更改后面两个的顺序， 会出现奇怪的图案
    glBegin(GL_QUADS);
      glVertex2i(410, 5);
      glVertex2i(440, 5);
      glVertex2i(450, 45);
      glVertex2i(400, 45);

      glVertex2i(400, 55);
      glVertex2i(450, 55);
      glVertex2i(440, 100);
      glVertex2i(410, 100);
    glEnd();

    // Note that the order in which vertices are used to construct a quadrilateral from strip data is different from that used with independent data.
    glBegin(GL_QUAD_STRIP);
      glVertex2i(410+50, 5);
      glVertex2i(440+50, 5);
      glVertex2i(400+50, 45);
      glVertex2i(450+50, 45);

      glVertex2i(400+50, 55);
      glVertex2i(450+50, 55);
      glVertex2i(410+50, 100);
      glVertex2i(440+50, 100);
    glEnd();

    // vertex array https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glVertexPointer.xml
    GLint vertices1[] = {410 + 100, 5,
                         440 + 100, 5,
                         400 + 100, 45,
                         450 + 100, 45,
                         400 + 100, 55,
                         450 + 100, 55,
                         410 + 100, 100,
                         440 + 100, 100};

    glVertexPointer(2, GL_INT, 0, vertices1);
    GLubyte vertice_index1[] = {0, 1, 2, 3,
                                4, 5, 6, 7};
    glDrawElements(GL_QUAD_STRIP, 8, GL_UNSIGNED_BYTE, vertice_index1);

    // bitmap https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glBitmap.xml
    GLubyte bitShape[20] = {
      0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00,
      0xff, 0x80, 0x7f, 0x00, 0x3e, 0x00, 0x1c, 0x00, 0x08, 0x00
    };
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glRasterPos2i(600, 100);
    glBitmap(9, 10, 0.0, 0.0, 20.0, 15.0, bitShape);

    // pixmap https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/
    int row = 50;
    int column = 50;
    int color = 0;
    int number = 0;
    int size = row * column;
    GLuint* pixMap = new GLuint[size]();
    for (int i=0; i<row; i++)
      for (int j=0; j<column; j++) {
        number = j + (i * column);
        color = int(255 * number / size);
        *(pixMap + number) = (color << 24) | (color << 16) | (color << 8) | 0xff;
      }
    glDrawPixels(50, 50, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8	, pixMap);
    delete[] pixMap;

    SDL_GL_SwapWindow(window);

    SDL_PollEvent(&event);
  } // Check if the ESC key was pressed or the window was closed
  while(event.type != SDL_QUIT);

  // Once finished with OpenGL functions, the SDL_GLContext can be deleted.
  SDL_GL_DeleteContext(glcontext);

  // Close and destroy the window
  SDL_DestroyWindow(window);

  // Clean up
  SDL_Quit();

  return 0;
}
