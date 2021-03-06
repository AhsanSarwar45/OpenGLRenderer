#version 420 core

struct PointLight
{
    vec4 position;

    vec4 color;

    float power;

    float shadowBias;
    float shadowNearClip;
    float shadowFarClip;
};

layout(std140, binding = 4) uniform PointLightTransform { mat4 lightSpaceVPMatrix[600]; }
pointLightTransform;
layout(std140, binding = 5) uniform PointLightArray { PointLight pointLights[100]; }
pointLightArray;

layout(triangles, invocations = 18) in;
layout(triangle_strip, max_vertices = 3) out;

out vec4     fragPos;
flat out int lightIndex;

out gl_PerVertex { vec4 gl_Position; };

void main()
{
    lightIndex = gl_InvocationID / 6;
    // vec3 normal     = cross(fragData[2].worldPos - fragData[0].worldPos, fragData[0].worldPos - fragData[1].worldPos);
    // vec3 lightDir   = pointLightArray.pointLights[lightIndex].position.xyz - fragData[0].worldPos;

    // if (dot(normal, lightDir) > 0.f)
    // {

    // for (int face = 0; face < 6; ++face)
    // {
    int layerIndex = gl_InvocationID;
    // vec4 vertex[3];
    // int  outOfBound[6] = {0, 0, 0, 0, 0, 0};
    // for (int i = 0; i < 3; ++i)
    // {
    //     vertex[i] = pointLightTransform.lightSpaceVPMatrix[layerIndex] * vec4(fragData[i].worldPos, 1.0);
    //     if (vertex[i].x > +vertex[i].w)
    //         ++outOfBound[0];
    //     if (vertex[i].x < -vertex[i].w)
    //         ++outOfBound[1];
    //     if (vertex[i].y > +vertex[i].w)
    //         ++outOfBound[2];
    //     if (vertex[i].y < -vertex[i].w)
    //         ++outOfBound[3];
    //     if (vertex[i].z > +vertex[i].w)
    //         ++outOfBound[4];
    //     if (vertex[i].z < -vertex[i].w)
    //         ++outOfBound[5];
    // }

    // bool inFrustum = true;
    // for (int i = 0; i < 6; ++i)
    //     if (outOfBound[i] == 3)
    //         inFrustum = false;

    // if (inFrustum)
    // {
    //     for (int i = 0; i < 3; ++i)
    //     {
    //         gl_Position = vertex[i];
    //         gl_Layer    = gl_InvocationID;
    //         EmitVertex();
    //     }
    //     EndPrimitive();
    // }

    for (int i = 0; i < 3; ++i)
    {
        fragPos     = gl_in[i].gl_Position;
        gl_Position = pointLightTransform.lightSpaceVPMatrix[layerIndex] * fragPos;

        gl_Layer = layerIndex;
        EmitVertex();
    }
    EndPrimitive();
    // }
    // }
    // }
    // }
}
