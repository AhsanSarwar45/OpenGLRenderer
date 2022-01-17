#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform vec3 billboardPos;   // Position of the center of the billboard
uniform vec2 billboardScale; // Size of the billboard, in world units (probably meters)

out VertexData { out vec2 TexCoords; }
vertexData;

layout(std140) uniform Camera
{
    mat4 projection;
    mat4 view;
};

void main()
{
    vec3 cameraRightWorldSpace = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUpWorldSpace    = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 vertexPositionWorldspace =
        billboardPos + cameraRightWorldSpace * aPos.x * billboardScale.x + cameraUpWorldSpace * aPos.y * billboardScale.y;

    vertexData.TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(vertexPositionWorldspace, 1.0f);
}