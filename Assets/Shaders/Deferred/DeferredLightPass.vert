#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

layout(location = 0) out vec2 texCoords;
layout(location = 1) flat out int lightIndex;

void main(void)
{
    /* pass through instance ID */
    lightIndex  = gl_InstanceID;
    texCoords   = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}
