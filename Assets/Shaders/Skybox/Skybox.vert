#version 430 core
layout(location = 0) in vec3 aPos;

out vec3 TexCoords;

layout(std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
}
camera;

void main()
{
    TexCoords   = aPos;
    vec4 pos    = camera.projection * mat4(mat3(camera.view)) * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}