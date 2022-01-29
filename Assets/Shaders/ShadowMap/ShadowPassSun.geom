#version 420 core

struct SunLight
{
    vec3 direction;
};

layout(std140, binding = 2) uniform SunLightTransform { mat4 lightSpaceVPMatrix[600]; }
sunLightTransform;
layout(std140, binding = 3) uniform LightArray { SunLight sunLights[100]; }
lightArray;

// layout(std140, binding = 1) uniform lightArray { vec4 lightPosition[100]; }
// LightArray;

layout(triangles, invocations = 10) in;
layout(triangle_strip, max_vertices = 3) out;

in VertexData
{
    in vec3 worldPos;
    in vec3 normal;
}
fragData[];

out gl_PerVertex { vec4 gl_Position; };

void main()
{

    vec3 normal   = cross(fragData[2].worldPos - fragData[0].worldPos, fragData[0].worldPos - fragData[1].worldPos);
    vec3 lightDir = lightArray.sunLights[gl_InvocationID].direction;

    if (dot(normal, lightDir) > 0.f)
    {
        int layerIndex = gl_InvocationID;

        vec4 vertex[3];
        int  outOfBound[6] = {0, 0, 0, 0, 0, 0};
        for (int i = 0; i < 3; ++i)
        {
            vertex[i] = sunLightTransform.lightSpaceVPMatrix[layerIndex] * vec4(fragData[i].worldPos, 1.0);
            if (vertex[i].x > +vertex[i].w)
                ++outOfBound[0];
            if (vertex[i].x < -vertex[i].w)
                ++outOfBound[1];
            if (vertex[i].y > +vertex[i].w)
                ++outOfBound[2];
            if (vertex[i].y < -vertex[i].w)
                ++outOfBound[3];
            if (vertex[i].z > +vertex[i].w)
                ++outOfBound[4];
            if (vertex[i].z < -vertex[i].w)
                ++outOfBound[5];
        }

        bool inFrustum = true;
        for (int i = 0; i < 6; ++i)
            if (outOfBound[i] == 3)
                inFrustum = false;

        if (inFrustum)
        {
            for (int i = 0; i < 3; ++i)
            {
                gl_Position = vertex[i];
                gl_Layer    = gl_InvocationID;
                EmitVertex();
            }
            EndPrimitive();
        }
    }
}
