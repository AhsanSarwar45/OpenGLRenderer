
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;

out VertexData
{
    out vec3 worldPos;
    out vec3 normal;
}
vertexData;

void main()
{
    vertexData.normal   = aNormal;
    vertexData.worldPos = (model * vec4(aPos, 1.0)).xyz;
}