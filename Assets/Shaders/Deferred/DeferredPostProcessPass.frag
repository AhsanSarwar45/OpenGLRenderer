#version 420 core
out vec4 FragColor;

layout(location = 0) in vec2 texCoords;

layout(binding = 0) uniform sampler2D hdrBuffer;
uniform float exposure;

void main()
{
    const float gamma    = 2.2;
    vec3        hdrColor = texture(hdrBuffer, texCoords).rgb;
    // reinhard
    // vec3 result = hdrColor / (hdrColor + vec3(1.0));
    // exposure
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it
    result    = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}