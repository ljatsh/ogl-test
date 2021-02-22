
#include <stdio.h>

#include "SDL.h"
#include "SDL_opengl.h"

void show_gl_info() {
  fprintf(stdout, "gl information:\n");
  fprintf(stdout, "vendor:%s\n", glGetString(GL_VENDOR));
  fprintf(stdout, "renderer:%s\n", glGetString(GL_RENDERER));
  fprintf(stdout, "version:%s\n", glGetString(GL_VERSION));
  fprintf(stdout, "glsl version:%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
  fprintf(stdout, "extensions:%s\n", glGetString(GL_EXTENSIONS));
}

int main(int argc, const char* argv[])
{
  SDL_Window *window;                    // Declare a pointer

  SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2

  // Create an application window with the following settings:
  window = SDL_CreateWindow(
      "An SDL2 window",                  // window title
      SDL_WINDOWPOS_UNDEFINED,           // initial x position
      SDL_WINDOWPOS_UNDEFINED,           // initial y position
      640,                               // width, in pixels
      480,                               // height, in pixels
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

  glViewport(0, 0, 640, 480);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0, 640, 480, 0, 1, -1);

  glMatrixMode(GL_MODELVIEW);

  glEnable(GL_TEXTURE_2D);

  glLoadIdentity();

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  SDL_Event event;
  do{
    // // Clear the screen
    // glClear(GL_COLOR_BUFFER_BIT);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
 
    // glBegin(GL_QUADS);
    //     glColor3f(1, 0, 0); glVertex3f(0, 0, 0);
    //     glColor3f(1, 1, 0); glVertex3f(100, 0, 0);
    //     glColor3f(1, 0, 1); glVertex3f(100, 100, 0);
    //     glColor3f(1, 1, 1); glVertex3f(0, 100, 0);
    // glEnd();

    glBegin(GL_POINTS);
      glVertex2i(50, 100);
      glVertex2i(75, 150);
    glEnd();
 
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
