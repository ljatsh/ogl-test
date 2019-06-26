
#include "helper.h"
#include "shader.h"

GLuint programID;
GLuint vertexbuffer;
GLuint uvbuffer;
GLuint normalbuffer;

GLuint MatrixID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
GLuint LightID;

GLuint vertexPosition_modelspaceID;
GLuint vertexUVID;
GLuint vertexNormal_modelspaceID;

GLuint Texture;
GLuint TextureID;
std::vector<glm::vec3> vertices;

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
  programID = load_shaders("shader/vertex_12.vsh", "shader/frag_12.fsh");

  // Get a handle for our "MVP" uniform
  MatrixID = glGetUniformLocation(programID, "MVP");
  ViewMatrixID = glGetUniformLocation(programID, "V");
  ModelMatrixID = glGetUniformLocation(programID, "M");

  // Get a handle for our buffers
  vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
  vertexUVID = glGetAttribLocation(programID, "vertexUV");
  vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");

  // Load the texture
  Texture = load_dds("texture/uvmap2.DDS");
  
  // Get a handle for our "myTextureSampler" uniform
  TextureID  = glGetUniformLocation(programID, "myTextureSampler");

  // Read our .obj file
  std::vector<glm::vec2> uvs;
  std::vector<glm::vec3> normals;
  bool res = load_obj("model/suzanne.obj", vertices, uvs, normals);

  // Load it into a VBO
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &uvbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

  glGenBuffers(1, &normalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

  // Get a handle for our "LightPosition" uniform
  glUseProgram(programID);
  LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
}

void
display(GLFWwindow* wnd) {
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
  glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

  glm::vec3 lightPos = glm::vec3(4,4,4);
  glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

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
    GL_FALSE,                     // normalized?
    0,                            // stride
    (void*)0                      // array buffer offset
  );

  // 3rd attribute buffer : normals
  glEnableVertexAttribArray(vertexNormal_modelspaceID);
  glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  glVertexAttribPointer(
    vertexNormal_modelspaceID,    // The attribute we want to configure
    3,                            // size
    GL_FLOAT,                     // type
    GL_FALSE,                     // normalized?
    0,                            // stride
    (void*)0                      // array buffer offset
  );

  // Draw the triangles !
  glDrawArrays(GL_TRIANGLES, 0, vertices.size());

  glDisableVertexAttribArray(vertexPosition_modelspaceID);
  glDisableVertexAttribArray(vertexUVID);
  glDisableVertexAttribArray(vertexNormal_modelspaceID);
}

int
main() {
  int ret = run(1024, 768, "Tutorial 14 -- Basic Shading", init, display);

  glDeleteProgram(programID);
  glDeleteBuffers(1, &vertexbuffer);
  glDeleteBuffers(1, &uvbuffer);
  glDeleteBuffers(1, &normalbuffer);
  glDeleteTextures(1, &Texture);

  return ret;
}