#version 430 core

struct PointLight
{
    vec3  position;
    float shadowFarClip;
};

layout(std140, binding = 4) uniform LightArray { PointLight pointLights[100]; }
lightArray;

in vec4 FragPos;

void main()
{
    PointLight pointLight    = lightArray.pointLights[0];
    float      lightDistance = length(FragPos.xyz - pointLight.position);

    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / pointLight.shadowFarClip;

    // write this as modified depth
    gl_FragDepth = lightDistance;
}