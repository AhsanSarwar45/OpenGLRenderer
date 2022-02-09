#version 430 core
out vec4 FragColor;

uniform sampler2D gAlbedoAO;

layout(location = 0) in vec2 texCoord;

uniform vec3 ambientLight;

void main()
{
    vec4  albedoAO = texture(gAlbedoAO, texCoord);
    vec3  albedo   = albedoAO.rgb;
    float ao       = albedoAO.a;

    vec3 color = ambientLight * albedo * ao;

    FragColor = vec4(color, 1.0);
}