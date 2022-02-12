#version 430 core
out vec4 FragColor;

layout(binding = 15) uniform sampler2D gPositionMetalness;

struct SunLight
{
    vec4  position;
    vec4  direction;
    vec4  color;
    float power;
    float shadowBias;
};

layout(std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
    vec4 position;
}
camera;

layout(std140, binding = 3) uniform SunLightArray { SunLight sunLights[20]; }
sunLightArray; // todo rename to block

layout(location = 0) in vec2 texCoord;

uniform float shadowCascadeDistances[5];
uniform int   shadowCascadeCount;

uniform int lightIndex;

const vec3 colors[5] = {vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1), vec3(1, 0, 1), vec3(0, 1, 1)};

void main()
{
    // retrieve data from gbuffer
    vec3 fragPos = texture(gPositionMetalness, texCoord).xyz;

    vec4  fragPosViewSpace = camera.view * vec4(fragPos, 1.0);
    float depthValue       = abs(fragPosViewSpace.z);

    int layer;
    for (int i = 0; i <= shadowCascadeCount; ++i)
    {
        if (depthValue <= shadowCascadeDistances[i])
        {
            layer = i;
            break;
        }
    }

    // FragColor = vec4(vec3(depthValue / shadowCascadeDistances[4]), 1.0);

    //     if (depthValue / shadowCascadeDistances[4] < 1)
    // {
    FragColor = vec4(colors[layer], 1.0);
    // }
    // else
    // {
    //     FragColor = vec4(0.0);
    // }
}