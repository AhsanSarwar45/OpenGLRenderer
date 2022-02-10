#version 430 core

struct PointLight
{
    vec4 position;

    vec4 color;

    float power;

    float shadowBias;
    float shadowNearClip;
    float shadowFarClip;
};

layout(std140, binding = 5) uniform LightArray { PointLight pointLights[100]; }
lightArray;

in vec4     fragPos;
flat in int lightIndex;

void main()
{
    PointLight pointLight    = lightArray.pointLights[lightIndex];
    float      lightDistance = length(fragPos.xyz - pointLight.position.xyz);

    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / pointLight.shadowFarClip;

    // write this as modified depth
    gl_FragDepth = lightDistance;
}