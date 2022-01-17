#version 420 core

layout(std140, binding = 0) uniform lightTransform { mat4 VPMatrix[32]; }
LightTransform;

layout(triangles, invocations = 32) in;
layout(triangle_strip, max_vertices = 3) out;
layout(location = 0) in vec4 vertexPosition[];

out gl_PerVertex { vec4 gl_Position; };

void main()
{
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = LightTransform.VPMatrix[gl_InvocationID] * vertexPosition[i];
        gl_Layer    = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}