#version 330 core
out vec4 FragColor;

in VertexData { vec2 TexCoords; }
fragData;

struct Material
{
    sampler2D albedo;
};

uniform vec3 viewPos;

uniform Material material;

void main() { FragColor = texture(material.albedo, fragData.TexCoords); }