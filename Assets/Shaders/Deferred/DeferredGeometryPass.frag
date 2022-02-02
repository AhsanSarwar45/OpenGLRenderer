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

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metalnessMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPosition = fragData.FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = texture(normalMap, fragData.TexCoords).rgb;
    gNormal = normalize(gNormal * 2.0 - 1.0);
    gNormal = normalize(fragData.TBN * gNormal);

    gAlbedo = texture(albedoMap, fragData.TexCoords).rgb;

    gMetalnessRoughnessAO.r = texture(metalnessMap, fragData.TexCoords).r;
    gMetalnessRoughnessAO.g = texture(roughnessMap, fragData.TexCoords).r;
    gMetalnessRoughnessAO.b = texture(aoMap, fragData.TexCoords).r;
}