#version 330 core
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec3 gAlbedo;
layout(location = 3) out vec3 gMetalnessRoughnessAO;

in VertexData
{
    vec2 TexCoords;
    vec3 FragPos;
    mat3 TBN;
    vec4 FragPosLightSpace;
}
fragData;

struct Material
{
    sampler2D albedo;
    sampler2D normal;
    sampler2D metalness;
    sampler2D roughness;
    sampler2D ao;
};

uniform Material material;

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPosition = fragData.FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = texture(material.normal, fragData.TexCoords).rgb;
    gNormal = normalize(gNormal * 2.0 - 1.0);
    gNormal = normalize(fragData.TBN * gNormal);

    gAlbedo = pow(texture(material.albedo, fragData.TexCoords).rgb, vec3(2.2));
    // gAlbedo = texture(material.albedo, fragData.TexCoords).rgb;

    gMetalnessRoughnessAO.r = texture(material.metalness, fragData.TexCoords).r;
    gMetalnessRoughnessAO.g = texture(material.roughness, fragData.TexCoords).r;
    gMetalnessRoughnessAO.b = texture(material.ao, fragData.TexCoords).r;
}