
#include "helper.h"
#include "shader.h"

GLuint programID;
GLuint vertexbuffer;
GLuint uvbuffer;
GLuint MatrixID;
GLuint Texture;
GLuint TextureID;
GLuint vertexPosition_modelspaceID;
GLuint vertexUVID;
int verticesSize;

void
init(GLFWwindow* wnd) {
  // Set the mouse at the center of the screen
  int width, height;
  glfwGetWindowSize(wnd, &width, &height);
  glfwPollEvents();
  glfwSetCursorPos(wnd, width/2, height/2);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS); 

  // Cull triangles which normal is not towards the camera
  glEnable(GL_CULL_FACE);

  // Create and compile our GLSL program from the shaders
  programID = load_shaders( "shader/vertex_11.vsh", "shader/frag_11.fsh" );

  // Get a handle for our "MVP" uniform
  MatrixID = glGetUniformLocation(programID, "MVP");

  // Get a handle for our buffers
  vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
  vertexUVID = glGetAttribLocation(programID, "vertexUV");

  // Load the texture
  Texture = load_dds("texture/uvmap.DDS");
  
  // Get a handle for our "myTextureSampler" uniform
  TextureID = glGetUniformLocation(programID, "myTextureSampler");

  // Read our .obj file
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> uvs;
  std::vector<glm::vec3> normals; // Won't be used at the moment.
  bool res = load_obj("model/cube.obj", vertices, uvs, normals);
  verticesSize = vertices.size();

  // Load it into a VBO
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &uvbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
}

void display(GLFWwindow* wnd) {
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Use our shader
  glUseProgram(programID);

  // Compute the MVP matrix from keyboard and mouse input
  refresh_matrix_from_control(wnd);
  glm::mat4 ProjectionMatrix = get_current_projection_matrix();
  glm::mat4 ViewMatrix = get_current_view_matrix();
  glm::mat4 ModelMatrix = glm::mat4(1.0);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

  // Send our transformation to the currently bound shader, 
  // in the "MVP" uniform
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // Bind our texture in Texture Unit 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, Texture);
  // Set our "myTextureSampler" sampler to user Texture Unit 0
  glUniform1i(TextureID, 0);

  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(vertexPosition_modelspaceID);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glVertexAttribPointer(
    vertexPosition_modelspaceID,  // The attribute we want to configure
    3,                            // size
    GL_FLOAT,                     // type
    GL_FALSE,                     // normalized?
    0,                            // stride
    (void*)0                      // array buffer offset
  );

  // 2nd attribute buffer : UVs
  glEnableVertexAttribArray(vertexUVID);
  glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
  glVertexAttribPointer(
    vertexUVID,                   // The attribute we want to configure
    2,                            // size : U+V => 2
    GL_FLOAT,                     // type
    GL_FALSE,                     // normalized
    0,                            // stride
    (void*)0                      // array buffer offset
  );

  // Draw the triangles !
  glDrawArrays(GL_TRIANGLES, 0, verticesSize);

  glDisableVertexAttribArray(vertexPosition_modelspaceID);
  glDisableVertexAttribArray(vertexUVID);
}

int
main(void)
{
  int ret = run(1024, 768, "Tutorial 11 - Blender Model", init, display);

  glDeleteProgram(programID);
  glDeleteBuffers(1, &vertexbuffer);
  glDeleteBuffers(1, &uvbuffer);
  glDeleteTextures(1, &Texture);

  return ret;
}
