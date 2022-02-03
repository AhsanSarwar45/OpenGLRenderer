#version 430 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;

layout(std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
}
camera;

void main() { gl_Position = camera.projection * camera.view * model * vec4(aPos, 1.0); }