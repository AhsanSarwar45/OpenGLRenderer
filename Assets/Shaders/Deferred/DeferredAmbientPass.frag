#version 430 core
out vec4 FragColor;

uniform sampler2D gAlbedo;
uniform sampler2D gMetalnessRoughnessAO;

layout(location = 0) in vec2 texCoord;

uniform vec3 ambientLight;

void main()
{
    vec3  albedo               = texture(gAlbedo, texCoord).rgb;
    vec3  metalnessRoughnessAO = texture(gMetalnessRoughnessAO, texCoord).rgb;
    float ao                   = metalnessRoughnessAO.b;

    vec3 color = ambientLight * albedo * ao;

    FragColor = vec4(color, 1.0);
}