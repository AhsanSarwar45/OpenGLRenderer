#version 420 core

struct PointLight
{
    vec3  position;
    float shadowFarClip;
};

layout(std140, binding = 3) uniform LightTransform { mat4 lightSpaceVPMatrix[600]; }
lightTransform;
layout(std140, binding = 4) uniform LightArray { PointLight pointLights[100]; }
lightArray;

// layout(std140, binding = 1) uniform lightArray { vec4 lightPosition[100]; }
// LightArray;

layout(triangles, invocations = 10) in;
layout(triangle_strip, max_vertices = 18) out;

in VertexData
{
    in vec3 worldPos;
    in mat3 TBN;
}
fragData[];

out vec4 FragPos;

out gl_PerVertex { vec4 gl_Position; };

void main()
{

    // vec3 light  = vec3(LightArray.light[gl_InvocationID].position) - outVertexPosition[0];

    // if (dot(normal, light) > 0.f)
    // {

    for (int face = 0; face < 6; ++face)
    {
        int layerIndex = 6 * gl_InvocationID + face;
        for (int i = 0; i < 3; ++i)
        {
            FragPos     = vec4(fragData[i].worldPos, 1.0);
            gl_Position = lightTransform.lightSpaceVPMatrix[layerIndex] * FragPos;
            gl_Layer    = layerIndex;
            EmitVertex();
        }
        EndPrimitive();

        // }
    }
}
