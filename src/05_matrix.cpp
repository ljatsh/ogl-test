
#include "helper.h"
#include "shader.h"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

GLuint vertexbuffer;
GLuint vertexPosition_modelspaceID = 0;
GLuint programID;

// Model Matrix:
// 1) Translation
//    | 1   0   0   X |
//    | 0   1   0   Y |
//    | 0   0   1   Z |
//    | 0   0   0   1 |
// 2) Scaling
//    | x   0   0   0 |
//    | 0   y   0   0 |
//    | 0   0   z   0 |
//    | 0   0   0   1 |
// 3) Rotation  TBD
//
// View Matrix:
//

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
  programID = load_shaders("shader/vertex_05.vsh", "shader/frag_05.fsh");

  // Get a handle for our buffers
  vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");

  // Use shader
  glUseProgram(programID);

  // Get a handle for our "MVP" uniform
  GLuint MatrixID = glGetUniformLocation(programID, "MVP");
  // Projection matrix : 45∞ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
  // Or, for an ortho camera :
  //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
    
  // Camera matrix
  glm::mat4 View       = glm::lookAt(
                glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
                glm::vec3(0, 0, 0), // and looks at the origin
                glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
               );
  // Model matrix : an identity matrix (model will be at the origin)
  glm::mat4 Model      = glm::mat4(1.0f);
  // Our ModelViewProjection : multiplication of our 3 matrices
  glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

  // Send our transformation to the currently bound shader, 
  // in the "MVP" uniform
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

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
  glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

  glFlush();
}

int
main()
{
  int ret = run(1024, 768, "Tutorial 05 - Matrix", init, display);

  // Cleanup VBO
  glDeleteBuffers(1, &vertexbuffer);
  glDeleteProgram(programID);

  return ret;
}
