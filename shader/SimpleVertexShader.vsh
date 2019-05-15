#version 120

// Input vertex data, different for all executions of this shader.
attribute vec3 vertexPosition_modelspace;

void main(){
    gl_Position = vec4(vertexPosition_modelspace, 1.0);

    gl_Position.x = gl_Position.x * 0.5;
    gl_Position.y = gl_Position.y * 0.5;
}
