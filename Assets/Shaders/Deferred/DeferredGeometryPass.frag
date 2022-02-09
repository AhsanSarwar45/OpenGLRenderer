#version 330 core
layout(location = 0) out vec4 gPositionMetalness;
layout(location = 1) out vec4 gNormalRoughness;
layout(location = 2) out vec4 gAlbedoAO;
// layout(location = 3) out vec3 gMetalnessRoughnessAO;

in VertexData
{
    vec2 TexCoords;
    vec3 FragPos;
    mat3 TBN;
    vec4 FragPosLightSpace;
}
fragData;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metalnessMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPositionMetalness.rgb = fragData.FragPos;
    // also store the per-fragment normals into the gbuffer
    vec3 normal          = texture(normalMap, fragData.TexCoords).rgb;
    normal               = normalize(normal * 2.0 - 1.0);
    gNormalRoughness.rgb = normalize(fragData.TBN * normal);

    gAlbedoAO.rgb = texture(albedoMap, fragData.TexCoords).rgb;

    gPositionMetalness.a = texture(metalnessMap, fragData.TexCoords).r;
    gNormalRoughness.a   = texture(roughnessMap, fragData.TexCoords).r;
    gAlbedoAO.a          = texture(aoMap, fragData.TexCoords).r;
}