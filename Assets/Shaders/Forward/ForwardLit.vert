

#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
}
camera;

out VertexData
{
    out vec2 TexCoords;
    out vec3 FragPos;
    out mat3 TBN;
    out vec4 FragPosLightSpace;
}
vertexData;

uniform mat4 model;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);

    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));

    vertexData.FragPos   = worldPos.xyz;
    vertexData.TexCoords = aTexCoords;
    vertexData.TBN       = mat3(T, B, N);
    // vertexData.FragPosLightSpace = lightSpaceMatrix * vec4(vertexData.FragPos, 1.0);

    gl_Position = camera.projection * camera.view * worldPos;
}
