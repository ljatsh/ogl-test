
#include "helper.h"
#include "shader.h"

// 1. Screen Coordinate uses right hand rule:
//    * X is your thumb
//    * Y is your index
//    * Z is your middle finger
      
// 2.

GLuint vertexbuffer;
GLuint vertexPosition_modelspaceID = 0;
GLuint programID;

void
init()
{
  static const GLfloat g_vertex_buffer_data[] = { 
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     0.0f,  1.0f, 0.0f,
  };

  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

  // Create and compile our GLSL program from the shaders
  programID = load_shaders("shader/SimpleVertexShader.vsh", "shader/SimpleFragmentShader.fsh");

  // Get a handle for our buffers
  vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");

  // Use shader
  glUseProgram(programID);

  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(vertexPosition_modelspaceID);
  glVertexAttribPointer(
     vertexPosition_modelspaceID,     // The attribute we want to configure
     3,                               // size
     GL_FLOAT,                        // type
     GL_FALSE,                        // normalized?
     0,                               // stride
     (void*)0                         // array buffer offset
   );
}

void
display()
{
  // Draw the triangle !
  //glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

  glFlush();
}

int
main()
{
  int ret = run(1024, 768, "Tutorial 02 - Red Triangle", init, display);

  // Cleanup VBO
  glDeleteBuffers(1, &vertexbuffer);
  glDeleteProgram(programID);

  return ret;
}
