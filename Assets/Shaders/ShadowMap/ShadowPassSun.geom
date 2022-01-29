#version 420 core

layout(std140, binding = 2) uniform SunLightTransform { mat4 LightSpaceVPMatrix[600]; }
sunLightTransform;

// layout(std140, binding = 1) uniform lightArray { vec4 lightPosition[100]; }
// LightArray;

layout(triangles, invocations = 10) in;
layout(triangle_strip, max_vertices = 3) out;

in VertexData
{
    in vec3 worldPos;
    in mat3 TBN;
}
fragData[];

out gl_PerVertex { vec4 gl_Position; };

void main()
{

    // vec3 light  = vec3(LightArray.light[gl_InvocationID].position) - outVertexPosition[0];

    // if (dot(normal, light) > 0.f)
    // {

    int layerIndex = gl_InvocationID;
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = sunLightTransform.LightSpaceVPMatrix[gl_InvocationID] * vec4(fragData[i].worldPos, 1.0);
        gl_Layer    = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}
