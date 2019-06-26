
#include "helper.h"
#include "shader.h"

// TODO: bump mapping --> https://blog.csdn.net/biezhihua/article/details/78628710

GLuint vertexbuffer;
GLuint uvbuffer;
GLuint normalbuffer;
GLuint tangentbuffer;
GLuint bitangentbuffer;
GLuint elementbuffer;
GLuint programID;
GLuint DiffuseTexture;
GLuint NormalTexture;
GLuint SpecularTexture;

GLuint MatrixID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
GLuint ModelView3x3MatrixID;

GLuint vertexPosition_modelspaceID;
GLuint vertexUVID;
GLuint vertexNormal_modelspaceID;
GLuint vertexTangent_modelspaceID;
GLuint vertexBitangent_modelspaceID;

GLuint DiffuseTextureID;
GLuint NormalTextureID;
GLuint SpecularTextureID;

GLuint LightID;

double lastTime;
int nbFrames;

std::vector<unsigned short> indices;
std::vector<glm::vec3> indexed_vertices;
std::vector<glm::vec2> indexed_uvs;
std::vector<glm::vec3> indexed_normals;
std::vector<glm::vec3> indexed_tangents;
std::vector<glm::vec3> indexed_bitangents;

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
  programID = load_shaders("shader/vertex_16.vsh", "shader/frag_16.fsh");

  // Get a handle for our "MVP" uniform
  MatrixID = glGetUniformLocation(programID, "MVP");
  ViewMatrixID = glGetUniformLocation(programID, "V");
  ModelMatrixID = glGetUniformLocation(programID, "M");
  ModelView3x3MatrixID = glGetUniformLocation(programID, "MV3x3");

  // Get a handle for our buffers
  vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
  vertexUVID = glGetAttribLocation(programID, "vertexUV");
  vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");
  vertexTangent_modelspaceID = glGetAttribLocation(programID, "vertexTangent_modelspace");
  vertexBitangent_modelspaceID = glGetAttribLocation(programID, "vertexBitangent_modelspace");

  // Load the texture
  DiffuseTexture = load_dds("texture/diffuse.DDS");
  NormalTexture = load_dds("texture/normal.bmp");
  SpecularTexture = load_dds("texture/specular.DDS");
  
  // Get a handle for our "myTextureSampler" uniform
  DiffuseTextureID  = glGetUniformLocation(programID, "DiffuseTextureSampler");
  NormalTextureID  = glGetUniformLocation(programID, "NormalTextureSampler");
  SpecularTextureID  = glGetUniformLocation(programID, "SpecularTextureSampler");

  // Read our .obj file
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> uvs;
  std::vector<glm::vec3> normals;
  bool res = load_obj("model/cylinder.obj", vertices, uvs, normals);

  std::vector<glm::vec3> tangents;
  std::vector<glm::vec3> bitangents;
  compute_tagent_basics(
    vertices, uvs, normals, // input
    tangents, bitangents    // output
  );

  index_vbo_tbn(
    vertices, uvs, normals, tangents, bitangents, 
    indices, indexed_vertices, indexed_uvs, indexed_normals, indexed_tangents, indexed_bitangents
  );

  // Load it into a VBO
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &uvbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
  glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

  glGenBuffers(1, &normalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

  glGenBuffers(1, &tangentbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
  glBufferData(GL_ARRAY_BUFFER, indexed_tangents.size() * sizeof(glm::vec3), &indexed_tangents[0], GL_STATIC_DRAW);

  glGenBuffers(1, &bitangentbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
  glBufferData(GL_ARRAY_BUFFER, indexed_bitangents.size() * sizeof(glm::vec3), &indexed_bitangents[0], GL_STATIC_DRAW);

  // Generate a buffer for the indices as well
  glGenBuffers(1, &elementbuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

  // Get a handle for our "LightPosition" uniform
  glUseProgram(programID);
  LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

  // For speed computation
  lastTime = glfwGetTime();
  nbFrames = 0;
}

void
display(GLFWwindow* wnd) {
  // Measure speed
  double currentTime = glfwGetTime();
  nbFrames++;
  if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
    // printf and reset
    printf("%f ms/frame\n", 1000.0/double(nbFrames));
    nbFrames = 0;
    lastTime += 1.0;
  }

  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Use our shader
  glUseProgram(programID);

  // Compute the MVP matrix from keyboard and mouse input
  refresh_matrix_from_control(wnd);
  glm::mat4 ProjectionMatrix = get_current_projection_matrix();
  glm::mat4 ViewMatrix = get_current_view_matrix();
  glm::mat4 ModelMatrix = glm::mat4(1.0);
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

  // Send our transformation to the currently bound shader, 
  // in the "MVP" uniform
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
  glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
  glUniformMatrix3fv(ModelView3x3MatrixID, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
  

  glm::vec3 lightPos = glm::vec3(0,0,4);
  glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

  // Bind our diffuse texture in Texture Unit 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, DiffuseTexture);
  // Set our "DiffuseTextureSampler" sampler to user Texture Unit 0
  glUniform1i(DiffuseTextureID, 0);

  // Bind our normal texture in Texture Unit 1
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, NormalTexture);
  // Set our "Normal  TextureSampler" sampler to user Texture Unit 0
  glUniform1i(NormalTextureID, 1);

  // Bind our normal texture in Texture Unit 2
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, SpecularTexture);
  // Set our "Normal  TextureSampler" sampler to user Texture Unit 0
  glUniform1i(SpecularTextureID, 2);

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

  // 4th attribute buffer : tangents
  glEnableVertexAttribArray(vertexTangent_modelspaceID);
  glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
  glVertexAttribPointer(
    vertexTangent_modelspaceID,    // The attribute we want to configure
    3,                            // size
    GL_FLOAT,                     // type
    GL_FALSE,                     // normalized?
    0,                            // stride
    (void*)0                      // array buffer offset
  );

  // 5th attribute buffer : bitangents
  glEnableVertexAttribArray(vertexBitangent_modelspaceID);
  glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
  glVertexAttribPointer(
    vertexBitangent_modelspaceID,    // The attribute we want to configure
    3,                            // size
    GL_FLOAT,                     // type
    GL_FALSE,                     // normalized?
    0,                            // stride
    (void*)0                      // array buffer offset
  );

  // Index buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

  // Draw the triangles !
  glDrawElements(
    GL_TRIANGLES,      // mode
    indices.size(),    // count
    GL_UNSIGNED_SHORT, // type
    (void*)0           // element array buffer offset
  );

  glDisableVertexAttribArray(vertexPosition_modelspaceID);
  glDisableVertexAttribArray(vertexUVID);
  glDisableVertexAttribArray(vertexNormal_modelspaceID);
  glDisableVertexAttribArray(vertexTangent_modelspaceID);
  glDisableVertexAttribArray(vertexBitangent_modelspaceID);

  ////////////////////////////////////////////////////////
  // DEBUG ONLY !!!
  // Don't use this in real code !!
  ////////////////////////////////////////////////////////

  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf((const GLfloat*)&ProjectionMatrix[0]);
  glMatrixMode(GL_MODELVIEW);
  glm::mat4 MV = ViewMatrix * ModelMatrix;
  glLoadMatrixf((const GLfloat*)&MV[0]);

  glUseProgram(0);

  // normals
  glColor3f(0,0,1);
  glBegin(GL_LINES);
  for (unsigned int i=0; i<indices.size(); i++){
    glm::vec3 p = indexed_vertices[indices[i]];
    glVertex3fv(&p.x);
    glm::vec3 o = glm::normalize(indexed_normals[indices[i]]);
    p+=o*0.1f;
    glVertex3fv(&p.x);
  }
  glEnd();
  // tangents
  glColor3f(1,0,0);
  glBegin(GL_LINES);
  for (unsigned int i=0; i<indices.size(); i++){
    glm::vec3 p = indexed_vertices[indices[i]];
    glVertex3fv(&p.x);
    glm::vec3 o = glm::normalize(indexed_tangents[indices[i]]);
    p+=o*0.1f;
    glVertex3fv(&p.x);
  }
  glEnd();
  // bitangents
  glColor3f(0,1,0);
  glBegin(GL_LINES);
  for (unsigned int i=0; i<indices.size(); i++){
    glm::vec3 p = indexed_vertices[indices[i]];
    glVertex3fv(&p.x);
    glm::vec3 o = glm::normalize(indexed_bitangents[indices[i]]);
    p+=o*0.1f;
    glVertex3fv(&p.x);
  }
  glEnd();
  // light pos
  glColor3f(1,1,1);
  glBegin(GL_LINES);
    glVertex3fv(&lightPos.x);
    lightPos+=glm::vec3(1,0,0)*0.1f;
    glVertex3fv(&lightPos.x);
    lightPos-=glm::vec3(1,0,0)*0.1f;
    glVertex3fv(&lightPos.x);
    lightPos+=glm::vec3(0,1,0)*0.1f;
    glVertex3fv(&lightPos.x);
  glEnd();
}

int
main() {
  int ret = run(1024, 768, "Tutorial 16 -- Normal Mapping", init, display);

  glDeleteBuffers(1, &vertexbuffer);
  glDeleteBuffers(1, &uvbuffer);
  glDeleteBuffers(1, &normalbuffer);
  glDeleteBuffers(1, &tangentbuffer);
  glDeleteBuffers(1, &bitangentbuffer);
  glDeleteBuffers(1, &elementbuffer);
  glDeleteProgram(programID);
  glDeleteTextures(1, &DiffuseTexture);
  glDeleteTextures(1, &NormalTexture);
  glDeleteTextures(1, &SpecularTexture);

  return ret;
}
