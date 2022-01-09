#version 330 core
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

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
    sampler2D specular;
    sampler2D roughness;
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
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = pow(texture(material.albedo, fragData.TexCoords).rgb, vec3(2.2));
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(material.specular, fragData.TexCoords).r;
}