
#include "helper.h"
#include "shader.h"

#include <time.h>
#include <math.h>
#include <string.h>
#include <string>
#include <map>

#include <glm/gtc/matrix_transform.hpp>

// glfwGetWindowSize vs glfwGetFramebufferSize https://stackoverflow.com/questions/44719635/what-is-the-difference-between-glfwgetwindowsize-and-glfwgetframebuffersize

glm::mat4 g_current_view_matrix;
glm::mat4 g_current_projection_matrix;

void show_gl_info() {
  fprintf(stdout, "gl information:\n");
  fprintf(stdout, "vendor:%s\n", glGetString(GL_VENDOR));
  fprintf(stdout, "renderer:%s\n", glGetString(GL_RENDERER));
  fprintf(stdout, "version:%s\n", glGetString(GL_VERSION));
  fprintf(stdout, "glsl version:%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
  fprintf(stdout, "extensions:%s\n", glGetString(GL_EXTENSIONS));
}

void on_error(int error, const char* description) {
  fprintf(stderr, "glfw error %d: %s\n", error, description);
}

void on_position_updated(GLFWwindow* wnd, int x, int y) {
  fprintf(stdout, "GLFWwindow %p position was updated to %d, %d\n", wnd, x, y); 
}

void on_size_updated(GLFWwindow* wnd, int width, int height) {
  int w, h;
  glfwGetWindowSize(wnd, &w, &h);
  fprintf(stdout, "GLFWwindow %p size was updated to %d, %d\n", wnd, w, h);
  glfwGetFramebufferSize(wnd, &w, &h);
  fprintf(stdout, "GLFWwindow %p frame buffer size: %d, %d\n", wnd, w, h);
}

int run(int width, int height, const char* title, void (*init) (GLFWwindow* wnd), void (*display) (GLFWwindow* wnd)) {
  // Initialise GLFW
  if(!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n" );
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

  // Open a window and create its OpenGL context
  GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
  if(window == NULL) {
    fprintf(stderr, "Failed to open GLFW window.\n" );
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  glfwSetErrorCallback(on_error);
  glfwSetWindowPosCallback(window, on_position_updated);
  glfwSetWindowSizeCallback(window, on_size_updated);

  // Initialize GLEW
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    glfwTerminate();
    return -1;
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  show_gl_info();
  fprintf(stdout, "glew version:%s\n", glewGetString(GLEW_VERSION));

  if (init != nullptr)
    init(window);

  do{
    // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw
    if (display != nullptr)
      display(window);
    
    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  } // Check if the ESC key was pressed or the window was closed
  while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
       glfwWindowShouldClose(window) == 0 );

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}

void
init_rand() {
  srand(time(NULL));
}

int
rand(int min, int max) {
  float v = rand() * 1.0 / RAND_MAX;
  return min + floor((max - min) * v);
}

GLuint load_bmp(const char* path) {
  fprintf(stdout, "Reading image %s\n", path);

  // Data read from the header of the BMP file
  unsigned char header[54];
  unsigned int dataPos;
  unsigned int imageSize;
  unsigned int width, height;
  // Actual RGB data
  unsigned char * data;

  // Open the file
  FILE * file = fopen(path, "rb");
  if (!file) {
    fprintf(stderr, "%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", path);
    return 0;
  }

  // Read the header, i.e. the 54 first bytes

  // If less than 54 bytes are read, problem
  if ( fread(header, 1, 54, file)!=54 ){ 
    fprintf(stderr, "Not a correct BMP file\n");
    return 0;
  }

  // A BMP files always begins with "BM"
  if ( header[0]!='B' || header[1]!='M' ){
    fprintf(stderr, "Not a correct BMP file\n");
    return 0;
  }

  // Make sure this is a 24bpp file
  if ( *(int*)&(header[0x1E])!=0  )         {fprintf(stderr, "Not a correct BMP file\n");    return 0;}
  if ( *(int*)&(header[0x1C])!=24 )         {fprintf(stderr, "Not a correct BMP file\n");    return 0;}

  // Read the information about the image
  dataPos    = *(int*)&(header[0x0A]);
  imageSize  = *(int*)&(header[0x22]);
  width      = *(int*)&(header[0x12]);
  height     = *(int*)&(header[0x16]);

  // Some BMP files are misformatted, guess missing information
  if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
  if (dataPos==0)      dataPos=54; // The BMP header is done that way

  // Create a buffer
  data = new unsigned char [imageSize];

  // Read the actual data from the file into the buffer
  fread(data,1,imageSize,file);

  // Everything is in memory now, the file wan be closed
  fclose (file);

  // Create one OpenGL texture
  GLuint textureID;
  glGenTextures(1, &textureID);
  
  // "Bind" the newly created texture : all future texture functions will modify this texture
  glBindTexture(GL_TEXTURE_2D, textureID);

  // Give the image to OpenGL
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

  // OpenGL has now copied the data. Free our own version
  delete [] data;

  // Poor filtering, or ...
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

  // ... nice trilinear filtering.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
  glGenerateMipmap(GL_TEXTURE_2D);

  // Return the ID of the texture we just created
  return textureID;
}

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

GLuint load_dds(const char* path) {
  unsigned char header[124];

  FILE *fp; 
 
  /* try to open the file */ 
  fp = fopen(path, "rb"); 
  if (fp == NULL){
    fprintf(stderr, "%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", path);
    return 0;
  }
   
  /* verify the type of file */ 
  char filecode[4]; 
  fread(filecode, 1, 4, fp); 
  if (strncmp(filecode, "DDS ", 4) != 0) { 
    fclose(fp); 
    return 0; 
  }
  
  /* get the surface desc */ 
  fread(&header, 124, 1, fp); 

  unsigned int height      = *(unsigned int*)&(header[8 ]);
  unsigned int width       = *(unsigned int*)&(header[12]);
  unsigned int linearSize   = *(unsigned int*)&(header[16]);
  unsigned int mipMapCount = *(unsigned int*)&(header[24]);
  unsigned int fourCC      = *(unsigned int*)&(header[80]);

 
  unsigned char * buffer;
  unsigned int bufsize;
  /* how big is it going to be including all mipmaps? */ 
  bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize; 
  buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char)); 
  fread(buffer, 1, bufsize, fp); 
  /* close the file pointer */ 
  fclose(fp);

  unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4; 
  unsigned int format;
  switch(fourCC) 
  { 
  case FOURCC_DXT1: 
    format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; 
    break; 
  case FOURCC_DXT3: 
    format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; 
    break; 
  case FOURCC_DXT5: 
    format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; 
    break; 
  default: 
    free(buffer); 
    return 0; 
  }

  // Create one OpenGL texture
  GLuint textureID;
  glGenTextures(1, &textureID);

  // "Bind" the newly created texture : all future texture functions will modify this texture
  glBindTexture(GL_TEXTURE_2D, textureID);
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);  
  
  unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
  unsigned int offset = 0;

  /* load the mipmaps */ 
  for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) 
  { 
    unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize; 
    glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,  
      0, size, buffer + offset); 
   
    offset += size; 
    width  /= 2; 
    height /= 2; 

    // Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
    if(width < 1) width = 1;
    if(height < 1) height = 1;

  } 

  free(buffer); 

  return textureID;
}

// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 5 ); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

void refresh_matrix_from_control(GLFWwindow* wnd) {
  // glfwGetTime is called only once, the first time this function is called
  static double lastTime = glfwGetTime();

  // Compute time difference between current and last frame
  double currentTime = glfwGetTime();
  float deltaTime = float(currentTime - lastTime);

  // Get mouse position
  double xpos, ypos;
  glfwGetCursorPos(wnd, &xpos, &ypos);

  // Reset mouse position for next frame
  int width, height;
  glfwGetWindowSize(wnd, &width, &height);
  glfwSetCursorPos(wnd, width/2.0, height/2.0);

  // Compute new orientation
  horizontalAngle += mouseSpeed * deltaTime * float(width/2 - xpos );
  verticalAngle   += mouseSpeed * deltaTime * float(height/2 - ypos );

  // Direction : Spherical coordinates to Cartesian coordinates conversion
  glm::vec3 direction(
    cos(verticalAngle) * sin(horizontalAngle), 
    sin(verticalAngle),
    cos(verticalAngle) * cos(horizontalAngle)
  );
  
  // Right vector
  glm::vec3 right = glm::vec3(
    sin(horizontalAngle - 3.14f/2.0f), 
    0,
    cos(horizontalAngle - 3.14f/2.0f)
  );
  
  // Up vector
  glm::vec3 up = glm::cross(right, direction);

  // Move forward
  if (glfwGetKey(wnd, GLFW_KEY_UP) == GLFW_PRESS) {
    position += direction * deltaTime * speed;
  }
  // Move backward
  if (glfwGetKey(wnd, GLFW_KEY_DOWN) == GLFW_PRESS) {
    position -= direction * deltaTime * speed;
  }
  // Strafe right
  if (glfwGetKey(wnd, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    position += right * deltaTime * speed;
  }
  // Strafe left
  if (glfwGetKey(wnd, GLFW_KEY_LEFT) == GLFW_PRESS) {
    position -= right * deltaTime * speed;
  }

  float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

  // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  g_current_projection_matrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
  // Camera matrix
  g_current_view_matrix       = glm::lookAt(
                position,           // Camera is here
                position+direction, // and looks here : at the same position, plus "direction"
                up                  // Head is up (set to 0,-1,0 to look upside-down)
               );

  // For the next frame, the "last time" will be "now"
  lastTime = currentTime;
}

glm::mat4 get_current_view_matrix() {
  return g_current_view_matrix;
}

glm::mat4 get_current_projection_matrix() {
  return g_current_projection_matrix;
}

// Very, VERY simple OBJ loader.
// Here is a short list of features a real function would provide : 
// - Binary files. Reading a model should be just a few memcpy's away, not parsing a file at runtime. In short : OBJ is not very great.
// - Animations & bones (includes bones weights)
// - Multiple UVs
// - All attributes should be optional, not "forced"
// - More stable. Change a line in the OBJ file and it crashes.
// - More secure. Change another line and you can inject code.
// - Loading from memory, stream, etc

bool load_obj(
  const char * path, 
  std::vector<glm::vec3> & out_vertices, 
  std::vector<glm::vec2> & out_uvs,
  std::vector<glm::vec3> & out_normals
) {
  fprintf(stdout, "Loading OBJ file %s...\n", path);

  std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
  std::vector<glm::vec3> temp_vertices; 
  std::vector<glm::vec2> temp_uvs;
  std::vector<glm::vec3> temp_normals;

  FILE * file = fopen(path, "r");
  if( file == NULL ) {
    fprintf(stderr, "Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
    return false;
  }

  while(true) {
    char lineHeader[128];
    // read the first word of the line
    int res = fscanf(file, "%s", lineHeader);
    if (res == EOF)
      break; // EOF = End Of File. Quit the loop.

    // else : parse lineHeader
    
    if (strcmp( lineHeader, "v" ) == 0 ){
      glm::vec3 vertex;
      fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
      temp_vertices.push_back(vertex);
    }else if ( strcmp( lineHeader, "vt" ) == 0 ){
      glm::vec2 uv;
      fscanf(file, "%f %f\n", &uv.x, &uv.y );
      uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
      temp_uvs.push_back(uv);
    }else if ( strcmp( lineHeader, "vn" ) == 0 ){
      glm::vec3 normal;
      fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
      temp_normals.push_back(normal);
    }else if ( strcmp( lineHeader, "f" ) == 0 ){
      std::string vertex1, vertex2, vertex3;
      unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
      int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
      if (matches != 9){
        printf("File can't be read by our simple parser :-( Try exporting with other options\n");
        return false;
      }
      vertexIndices.push_back(vertexIndex[0]);
      vertexIndices.push_back(vertexIndex[1]);
      vertexIndices.push_back(vertexIndex[2]);
      uvIndices    .push_back(uvIndex[0]);
      uvIndices    .push_back(uvIndex[1]);
      uvIndices    .push_back(uvIndex[2]);
      normalIndices.push_back(normalIndex[0]);
      normalIndices.push_back(normalIndex[1]);
      normalIndices.push_back(normalIndex[2]);
    }else{
      // Probably a comment, eat up the rest of the line
      char stupidBuffer[1000];
      fgets(stupidBuffer, 1000, file);
    }
  }

  // For each vertex of each triangle
  for( unsigned int i=0; i<vertexIndices.size(); i++ ){

    // Get the indices of its attributes
    unsigned int vertexIndex = vertexIndices[i];
    unsigned int uvIndex = uvIndices[i];
    unsigned int normalIndex = normalIndices[i];
    
    // Get the attributes thanks to the index
    glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
    glm::vec2 uv = temp_uvs[ uvIndex-1 ];
    glm::vec3 normal = temp_normals[ normalIndex-1 ];
    
    // Put the attributes in buffers
    out_vertices.push_back(vertex);
    out_uvs     .push_back(uv);
    out_normals .push_back(normal);
  }

  return true;
}

// Returns true iif v1 can be considered equal to v2
bool is_near(float v1, float v2){
  return fabs( v1-v2 ) < 0.01f;
}

// Searches through all already-exported vertices
// for a similar one.
// Similar = same position + same UVs + same normal
bool get_similar_vertex_index(
  glm::vec3 & in_vertex, 
  glm::vec2 & in_uv, 
  glm::vec3 & in_normal, 
  std::vector<glm::vec3> & out_vertices,
  std::vector<glm::vec2> & out_uvs,
  std::vector<glm::vec3> & out_normals,
  unsigned short & result
){
  // Lame linear search
  for ( unsigned int i=0; i<out_vertices.size(); i++ ){
    if (
      is_near( in_vertex.x , out_vertices[i].x ) &&
      is_near( in_vertex.y , out_vertices[i].y ) &&
      is_near( in_vertex.z , out_vertices[i].z ) &&
      is_near( in_uv.x     , out_uvs     [i].x ) &&
      is_near( in_uv.y     , out_uvs     [i].y ) &&
      is_near( in_normal.x , out_normals [i].x ) &&
      is_near( in_normal.y , out_normals [i].y ) &&
      is_near( in_normal.z , out_normals [i].z )
    ){
      result = i;
      return true;
    }
  }
  // No other vertex could be used instead.
  // Looks like we'll have to add it to the VBO.
  return false;
}

void index_vbo_slow(
  std::vector<glm::vec3> & in_vertices,
  std::vector<glm::vec2> & in_uvs,
  std::vector<glm::vec3> & in_normals,

  std::vector<unsigned short> & out_indices,
  std::vector<glm::vec3> & out_vertices,
  std::vector<glm::vec2> & out_uvs,
  std::vector<glm::vec3> & out_normals
){
  // For each input vertex
  for ( unsigned int i=0; i<in_vertices.size(); i++ ){

    // Try to find a similar vertex in out_XXXX
    unsigned short index;
    bool found = get_similar_vertex_index(in_vertices[i], in_uvs[i], in_normals[i],     out_vertices, out_uvs, out_normals, index);

    if ( found ){ // A similar vertex is already in the VBO, use it instead !
      out_indices.push_back( index );
    }else{ // If not, it needs to be added in the output data.
      out_vertices.push_back( in_vertices[i]);
      out_uvs     .push_back( in_uvs[i]);
      out_normals .push_back( in_normals[i]);
      out_indices .push_back( (unsigned short)out_vertices.size() - 1 );
    }
  }
}

struct PackedVertex{
  glm::vec3 position;
  glm::vec2 uv;
  glm::vec3 normal;
  bool operator<(const PackedVertex that) const{
    return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
  };
};

bool get_similar_vertex_index_fast(
  PackedVertex & packed, 
  std::map<PackedVertex,unsigned short> & VertexToOutIndex,
  unsigned short & result
){
  std::map<PackedVertex,unsigned short>::iterator it = VertexToOutIndex.find(packed);
  if ( it == VertexToOutIndex.end() ){
    return false;
  }else{
    result = it->second;
    return true;
  }
}

void index_vbo(
  std::vector<glm::vec3> & in_vertices,
  std::vector<glm::vec2> & in_uvs,
  std::vector<glm::vec3> & in_normals,

  std::vector<unsigned short> & out_indices,
  std::vector<glm::vec3> & out_vertices,
  std::vector<glm::vec2> & out_uvs,
  std::vector<glm::vec3> & out_normals
){
  std::map<PackedVertex,unsigned short> VertexToOutIndex;

  // For each input vertex
  for ( unsigned int i=0; i<in_vertices.size(); i++ ){

    PackedVertex packed = {in_vertices[i], in_uvs[i], in_normals[i]};
    

    // Try to find a similar vertex in out_XXXX
    unsigned short index;
    bool found = get_similar_vertex_index_fast(packed, VertexToOutIndex, index);

    if ( found ){ // A similar vertex is already in the VBO, use it instead !
      out_indices.push_back( index );
    }else{ // If not, it needs to be added in the output data.
      out_vertices.push_back( in_vertices[i]);
      out_uvs     .push_back( in_uvs[i]);
      out_normals .push_back( in_normals[i]);
      unsigned short newindex = (unsigned short)out_vertices.size() - 1;
      out_indices .push_back( newindex );
      VertexToOutIndex[ packed ] = newindex;
    }
  }
}

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
){
  // For each input vertex
  for ( unsigned int i=0; i<in_vertices.size(); i++ ){

    // Try to find a similar vertex in out_XXXX
    unsigned short index;
    bool found = get_similar_vertex_index(in_vertices[i], in_uvs[i], in_normals[i],     out_vertices, out_uvs, out_normals, index);

    if ( found ){ // A similar vertex is already in the VBO, use it instead !
      out_indices.push_back( index );

      // Average the tangents and the bitangents
      out_tangents[index] += in_tangents[i];
      out_bitangents[index] += in_bitangents[i];
    }else{ // If not, it needs to be added in the output data.
      out_vertices.push_back( in_vertices[i]);
      out_uvs     .push_back( in_uvs[i]);
      out_normals .push_back( in_normals[i]);
      out_tangents .push_back( in_tangents[i]);
      out_bitangents .push_back( in_bitangents[i]);
      out_indices .push_back( (unsigned short)out_vertices.size() - 1 );
    }
  }
}

unsigned int Text2DTextureID;              // Texture containing the font
unsigned int Text2DVertexBufferID;         // Buffer containing the vertices
unsigned int Text2DUVBufferID;             //                       UVs
unsigned int Text2DShaderID;               // Program used to disaply the text
unsigned int vertexPosition_screenspaceID; // Location of the program's "vertexPosition_screenspace" attribute
unsigned int _vertexUVID;                  // Location of the program's "vertexUV" attribute
unsigned int Text2DUniformID;              // Location of the program's texture attribute

void init_text_2d(const char * texturePath){

  // Initialize texture
  Text2DTextureID = load_dds(texturePath);

  // Initialize VBO
  glGenBuffers(1, &Text2DVertexBufferID);
  glGenBuffers(1, &Text2DUVBufferID);

  // Initialize Shader
  Text2DShaderID = load_shaders("shader/text.vsh", "shader/text.fsh" );

  // Get a handle for our buffers
  vertexPosition_screenspaceID = glGetAttribLocation(Text2DShaderID, "vertexPosition_screenspace");
  _vertexUVID = glGetAttribLocation(Text2DShaderID, "vertexUV");

  // Initialize uniforms' IDs
  Text2DUniformID = glGetUniformLocation(Text2DShaderID, "myTextureSampler");
}

void print_text(const char * text, int x, int y, int size) {
  unsigned int length = strlen(text);

  // Fill buffers
  std::vector<glm::vec2> vertices;
  std::vector<glm::vec2> UVs;
  for ( unsigned int i=0 ; i<length ; i++ ){
    glm::vec2 vertex_up_left    = glm::vec2( x+i*size     , y+size );
    glm::vec2 vertex_up_right   = glm::vec2( x+i*size+size, y+size );
    glm::vec2 vertex_down_right = glm::vec2( x+i*size+size, y      );
    glm::vec2 vertex_down_left  = glm::vec2( x+i*size     , y      );

    vertices.push_back(vertex_up_left   );
    vertices.push_back(vertex_down_left );
    vertices.push_back(vertex_up_right  );

    vertices.push_back(vertex_down_right);
    vertices.push_back(vertex_up_right);
    vertices.push_back(vertex_down_left);

    char character = text[i];
    float uv_x = (character%16)/16.0f;
    float uv_y = (character/16)/16.0f;

    glm::vec2 uv_up_left    = glm::vec2( uv_x           , uv_y );
    glm::vec2 uv_up_right   = glm::vec2( uv_x+1.0f/16.0f, uv_y );
    glm::vec2 uv_down_right = glm::vec2( uv_x+1.0f/16.0f, (uv_y + 1.0f/16.0f) );
    glm::vec2 uv_down_left  = glm::vec2( uv_x           , (uv_y + 1.0f/16.0f) );
    UVs.push_back(uv_up_left   );
    UVs.push_back(uv_down_left );
    UVs.push_back(uv_up_right  );

    UVs.push_back(uv_down_right);
    UVs.push_back(uv_up_right);
    UVs.push_back(uv_down_left);
  }
  glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
  glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);

  // Bind shader
  glUseProgram(Text2DShaderID);

  // Bind texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, Text2DTextureID);
  // Set our "myTextureSampler" sampler to user Texture Unit 0
  glUniform1i(Text2DUniformID, 0);

  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(vertexPosition_screenspaceID);
  glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
  glVertexAttribPointer(vertexPosition_screenspaceID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

  // 2nd attribute buffer : UVs
  glEnableVertexAttribArray(_vertexUVID);
  glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
  glVertexAttribPointer(_vertexUVID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Draw call
  glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

  glDisable(GL_BLEND);

  glDisableVertexAttribArray(vertexPosition_screenspaceID);
  glDisableVertexAttribArray(_vertexUVID);
}

void clear_text() {
  // Delete buffers
  glDeleteBuffers(1, &Text2DVertexBufferID);
  glDeleteBuffers(1, &Text2DUVBufferID);

  // Delete texture
  glDeleteTextures(1, &Text2DTextureID);

  // Delete shader
  glDeleteProgram(Text2DShaderID);
}

void compute_tagent_basics(
  // inputs
  std::vector<glm::vec3> & vertices,
  std::vector<glm::vec2> & uvs,
  std::vector<glm::vec3> & normals,
  // outputs
  std::vector<glm::vec3> & tangents,
  std::vector<glm::vec3> & bitangents
){
  for (unsigned int i=0; i<vertices.size(); i+=3 ){
    // Shortcuts for vertices
    glm::vec3 & v0 = vertices[i+0];
    glm::vec3 & v1 = vertices[i+1];
    glm::vec3 & v2 = vertices[i+2];

    // Shortcuts for UVs
    glm::vec2 & uv0 = uvs[i+0];
    glm::vec2 & uv1 = uvs[i+1];
    glm::vec2 & uv2 = uvs[i+2];

    // Edges of the triangle : postion delta
    glm::vec3 deltaPos1 = v1-v0;
    glm::vec3 deltaPos2 = v2-v0;

    // UV delta
    glm::vec2 deltaUV1 = uv1-uv0;
    glm::vec2 deltaUV2 = uv2-uv0;

    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    glm::vec3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
    glm::vec3 bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;

    // Set the same tangent for all three vertices of the triangle.
    // They will be merged later, in vboindexer.cpp
    tangents.push_back(tangent);
    tangents.push_back(tangent);
    tangents.push_back(tangent);

    // Same thing for binormals
    bitangents.push_back(bitangent);
    bitangents.push_back(bitangent);
    bitangents.push_back(bitangent);
  }

  // See "Going Further"
  for (unsigned int i=0; i<vertices.size(); i+=1 )
  {
    glm::vec3 & n = normals[i];
    glm::vec3 & t = tangents[i];
    glm::vec3 & b = bitangents[i];
    
    // Gram-Schmidt orthogonalize
    t = glm::normalize(t - n * glm::dot(n, t));
    
    // Calculate handedness
    if (glm::dot(glm::cross(n, t), b) < 0.0f){
      t = t * -1.0f;
    }
  }
}

/*
 * Original teapot code copyright follows:
 */

/*
 * (c) Copyright 1993, Silicon Graphics, Inc.
 *
 * ALL RIGHTS RESERVED
 *
 * Permission to use, copy, modify, and distribute this software
 * for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Silicon
 * Graphics, Inc. not be used in advertising or publicity
 * pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU
 * "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR
 * OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  IN NO
 * EVENT SHALL SILICON GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE
 * ELSE FOR ANY DIRECT, SPECIAL, INCIDENTAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER,
 * INCLUDING WITHOUT LIMITATION, LOSS OF PROFIT, LOSS OF USE,
 * SAVINGS OR REVENUE, OR THE CLAIMS OF THIRD PARTIES, WHETHER OR
 * NOT SILICON GRAPHICS, INC.  HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * ARISING OUT OF OR IN CONNECTION WITH THE POSSESSION, USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * US Government Users Restricted Rights
 *
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer
 * Software clause at DFARS 252.227-7013 and/or in similar or
 * successor clauses in the FAR or the DOD or NASA FAR
 * Supplement.  Unpublished-- rights reserved under the copyright
 * laws of the United States.  Contractor/manufacturer is Silicon
 * Graphics, Inc., 2011 N.  Shoreline Blvd., Mountain View, CA
 * 94039-7311.
 *
 * OpenGL(TM) is a trademark of Silicon Graphics, Inc.
 */

/*
 * Rim, body, lid, and bottom data must be reflected in x and y;
 * handle and spout data across the y axis only.
 */
static int patchdata[][16] =
{
    { 102, 103, 104, 105,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15 }, /* rim    */
    {  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27 }, /* body   */
    {  24,  25,  26,  27,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40 },
    {  96,  96,  96,  96,  97,  98,  99, 100, 101, 101, 101, 101,   0,   1,   2,   3 }, /* lid    */
    {   0,   1,   2,   3, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117 },
    { 118, 118, 118, 118, 124, 122, 119, 121, 123, 126, 125, 120,  40,  39,  38,  37 }, /* bottom */
    {  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56 }, /* handle */
    {  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  28,  65,  66,  67 },
    {  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83 }, /* spout  */
    {  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95 }
};

static double cpdata[][3] =
{
    {0.2, 0, 2.7}, {0.2, -0.112, 2.7}, {0.112, -0.2, 2.7}, {0,
    -0.2, 2.7}, {1.3375, 0, 2.53125}, {1.3375, -0.749, 2.53125},
    {0.749, -1.3375, 2.53125}, {0, -1.3375, 2.53125}, {1.4375,
    0, 2.53125}, {1.4375, -0.805, 2.53125}, {0.805, -1.4375,
    2.53125}, {0, -1.4375, 2.53125}, {1.5, 0, 2.4}, {1.5, -0.84,
    2.4}, {0.84, -1.5, 2.4}, {0, -1.5, 2.4}, {1.75, 0, 1.875},
    {1.75, -0.98, 1.875}, {0.98, -1.75, 1.875}, {0, -1.75,
    1.875}, {2, 0, 1.35}, {2, -1.12, 1.35}, {1.12, -2, 1.35},
    {0, -2, 1.35}, {2, 0, 0.9}, {2, -1.12, 0.9}, {1.12, -2,
    0.9}, {0, -2, 0.9}, {-2, 0, 0.9}, {2, 0, 0.45}, {2, -1.12,
    0.45}, {1.12, -2, 0.45}, {0, -2, 0.45}, {1.5, 0, 0.225},
    {1.5, -0.84, 0.225}, {0.84, -1.5, 0.225}, {0, -1.5, 0.225},
    {1.5, 0, 0.15}, {1.5, -0.84, 0.15}, {0.84, -1.5, 0.15}, {0,
    -1.5, 0.15}, {-1.6, 0, 2.025}, {-1.6, -0.3, 2.025}, {-1.5,
    -0.3, 2.25}, {-1.5, 0, 2.25}, {-2.3, 0, 2.025}, {-2.3, -0.3,
    2.025}, {-2.5, -0.3, 2.25}, {-2.5, 0, 2.25}, {-2.7, 0,
    2.025}, {-2.7, -0.3, 2.025}, {-3, -0.3, 2.25}, {-3, 0,
    2.25}, {-2.7, 0, 1.8}, {-2.7, -0.3, 1.8}, {-3, -0.3, 1.8},
    {-3, 0, 1.8}, {-2.7, 0, 1.575}, {-2.7, -0.3, 1.575}, {-3,
    -0.3, 1.35}, {-3, 0, 1.35}, {-2.5, 0, 1.125}, {-2.5, -0.3,
    1.125}, {-2.65, -0.3, 0.9375}, {-2.65, 0, 0.9375}, {-2,
    -0.3, 0.9}, {-1.9, -0.3, 0.6}, {-1.9, 0, 0.6}, {1.7, 0,
    1.425}, {1.7, -0.66, 1.425}, {1.7, -0.66, 0.6}, {1.7, 0,
    0.6}, {2.6, 0, 1.425}, {2.6, -0.66, 1.425}, {3.1, -0.66,
    0.825}, {3.1, 0, 0.825}, {2.3, 0, 2.1}, {2.3, -0.25, 2.1},
    {2.4, -0.25, 2.025}, {2.4, 0, 2.025}, {2.7, 0, 2.4}, {2.7,
    -0.25, 2.4}, {3.3, -0.25, 2.4}, {3.3, 0, 2.4}, {2.8, 0,
    2.475}, {2.8, -0.25, 2.475}, {3.525, -0.25, 2.49375},
    {3.525, 0, 2.49375}, {2.9, 0, 2.475}, {2.9, -0.15, 2.475},
    {3.45, -0.15, 2.5125}, {3.45, 0, 2.5125}, {2.8, 0, 2.4},
    {2.8, -0.15, 2.4}, {3.2, -0.15, 2.4}, {3.2, 0, 2.4}, {0, 0,
    3.15}, {0.8, 0, 3.15}, {0.8, -0.45, 3.15}, {0.45, -0.8,
    3.15}, {0, -0.8, 3.15}, {0, 0, 2.85}, {1.4, 0, 2.4}, {1.4,
    -0.784, 2.4}, {0.784, -1.4, 2.4}, {0, -1.4, 2.4}, {0.4, 0,
    2.55}, {0.4, -0.224, 2.55}, {0.224, -0.4, 2.55}, {0, -0.4,
    2.55}, {1.3, 0, 2.55}, {1.3, -0.728, 2.55}, {0.728, -1.3,
    2.55}, {0, -1.3, 2.55}, {1.3, 0, 2.4}, {1.3, -0.728, 2.4},
    {0.728, -1.3, 2.4}, {0, -1.3, 2.4}, {0, 0, 0}, {1.425,
    -0.798, 0}, {1.5, 0, 0.075}, {1.425, 0, 0}, {0.798, -1.425,
    0}, {0, -1.5, 0.075}, {0, -1.425, 0}, {1.5, -0.84, 0.075},
    {0.84, -1.5, 0.075}
};

static double tex[2][2][2] =
{
    { {0.0, 0.0}, {1.0, 0.0} },
    { {0.0, 1.0}, {1.0, 1.0} }
};

static void darw_teapot(GLint grid, GLdouble scale, GLenum type)
{
    double p[4][4][3], q[4][4][3], r[4][4][3], s[4][4][3];
    long i, j, k, l;

    glPushAttrib( GL_ENABLE_BIT | GL_EVAL_BIT );
    glEnable( GL_AUTO_NORMAL );
    glEnable( GL_NORMALIZE );
    glEnable( GL_MAP2_VERTEX_3 );
    glEnable( GL_MAP2_TEXTURE_COORD_2 );

    glPushMatrix();
    glRotated( 270.0, 1.0, 0.0, 0.0 );
    glScaled( 0.5 * scale, 0.5 * scale, 0.5 * scale );
    glTranslated( 0.0, 0.0, -1.5 );

    for (i = 0; i < 10; i++) {
      for (j = 0; j < 4; j++) {
        for (k = 0; k < 4; k++) {
          for (l = 0; l < 3; l++) {
            p[j][k][l] = cpdata[patchdata[i][j * 4 + k]][l];
            q[j][k][l] = cpdata[patchdata[i][j * 4 + (3 - k)]][l];
            if (l == 1)
              q[j][k][l] *= -1.0;
            if (i < 6) {
              r[j][k][l] =
                cpdata[patchdata[i][j * 4 + (3 - k)]][l];
              if (l == 0)
                r[j][k][l] *= -1.0;
              s[j][k][l] = cpdata[patchdata[i][j * 4 + k]][l];
              if (l == 0)
                s[j][k][l] *= -1.0;
              if (l == 1)
                s[j][k][l] *= -1.0;
            }
          }
        }
      }

      glMap2d(GL_MAP2_TEXTURE_COORD_2, 0.0, 1.0, 2, 2, 0.0, 1.0, 4, 2,
        &tex[0][0][0]);
      glMap2d(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 4, 0.0, 1.0, 12, 4,
        &p[0][0][0]);
      glMapGrid2d(grid, 0.0, 1.0, grid, 0.0, 1.0);
      glEvalMesh2(type, 0, grid, 0, grid);
      glMap2d(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 4, 0.0, 1.0, 12, 4,
        &q[0][0][0]);
      glEvalMesh2(type, 0, grid, 0, grid);
      if (i < 6) {
        glMap2d(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 4, 0.0, 1.0, 12, 4,
          &r[0][0][0]);
        glEvalMesh2(type, 0, grid, 0, grid);
        glMap2d(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 4, 0.0, 1.0, 12, 4,
          &s[0][0][0]);
        glEvalMesh2(type, 0, grid, 0, grid);
      }
    }

    glPopMatrix();
    glPopAttrib();
}

void draw_wire_teapot(GLdouble size)
{
    /* We will use the general teapot rendering code */
    darw_teapot(10, size, GL_LINE);
}

void draw_solid_teapot(GLdouble size)
{
    /* We will use the general teapot rendering code */
    darw_teapot(7, size, GL_FILL);
}
