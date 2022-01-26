#version 430 core
out vec4 FragColor;

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

layout(binding = 12) uniform sampler2DArray shadowMapArray;

layout(std140, binding = 2) uniform lightTransform { mat4 LightSpaceVPMatrix[600]; }
LightTransform;

struct SunLight
{
    vec3  direction;
    vec3  color;
    float power;

    float shadowBias;
};

const int MAX_LIGHTS = 32;

uniform SunLight sunLights[MAX_LIGHTS];
uniform int      numSunLights;

const float PI = 3.14159265359;

uniform vec3 viewPos;

uniform vec3 ambientLight;

vec3 fresnelSchlick(float cosTheta, vec3 F0) { return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0); }

float DistributionGGX(vec3 normal, vec3 halfwayDir, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(normal, halfwayDir), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom       = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// vec3 CalculateLighting(PointLight pointLight, vec3 viewDir, vec3 lightDir, vec3 radiance, vec3 normal, vec3 albedo, float metallness,
//                        float roughness, vec3 F0)
// {
//     vec3 halfwayDir = normalize(viewDir + lightDir);

//     // cook-torrance brdf
//     float NDF = DistributionGGX(normal, halfwayDir, roughness);
//     float G   = GeometrySmith(normal, viewDir, lightDir, roughness);
//     vec3  F   = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);

//     vec3 kS = F;
//     vec3 kD = vec3(1.0) - kS;
//     kD *= 1.0 - metallness;

//     vec3  numerator   = NDF * G * F;
//     float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
//     vec3  specular    = numerator / denominator;

//     // add to outgoing radiance Lo
//     float NdotL = max(dot(normal, lightDir), 0.0);
//     return (kD * albedo / PI + specular) * radiance * NdotL;
// }

float CalculateShadow(float shadowBias, float dotLightNormal, int index)
{
    vec4 fragPosLightSpace = LightTransform.LightSpaceVPMatrix[index] * vec4(fragData.FragPos, 1.0);
    vec3 projCoords        = (fragPosLightSpace.xyz / fragPosLightSpace.w) * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    if (currentDepth > 1.0)
    {
        currentDepth = 1.0;
    }

    vec3 shadowMapTexelCoord = vec3(projCoords.x, projCoords.y, index);

    float closestDepth = texture(shadowMapArray, shadowMapTexelCoord).r;

    float bias = max(shadowBias * (1.0 - dotLightNormal), shadowBias / 10.0);

    float shadow = currentDepth > (closestDepth + bias) ? 1 : 0.0;

    // check whether current frag pos is in shadow
    // float shadow    = 0.0;
    // vec2  texelSize = 1.0 / textureSize(shadowMapArray, 0).xz;
    // for (int x = -1; x <= 1; ++x)
    // {
    //     for (int y = -1; y <= 1; ++y)
    //     {
    //         shadowMapTexelCoord = vec3(projCoords.xy + vec2(x, y) * texelSize, index);
    //         float pcfDepth      = texture(shadowMapArray, shadowMapTexelCoord).r;
    //         shadow += currentDepth > (pcfDepth + bias) ? 1.0 : 0.0;
    //     }
    // }
    // shadow /= 9.0;

    // if (projCoords.z > 1.0)
    //     shadow = 0.0;

    return shadow;
}

vec3 CalculateSunLighting(int index, vec3 viewDir, vec3 normal, vec3 albedo, float metallness, float roughness, vec3 F0)
{
    SunLight sunLight = sunLights[index];

    vec3 lightDir = normalize(sunLight.direction);

    vec3 halfwayDir = normalize(viewDir + lightDir);
    vec3 radiance   = sunLight.color * sunLight.power;

    // cook-torrance brdf
    float NDF = DistributionGGX(normal, halfwayDir, roughness);
    float G   = GeometrySmith(normal, viewDir, lightDir, roughness);
    vec3  F   = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallness;

    // add to outgoing radiance Lo
    float NdotL = max(dot(normal, lightDir), 0.0);

    vec3  numerator   = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * NdotL + 0.0001;
    vec3  specular    = numerator / denominator;

    float shadow   = CalculateShadow(sunLight.shadowBias, NdotL, index);
    vec3  lighting = (kD * albedo / PI + specular) * radiance * NdotL;

    return lighting * (1 - shadow);
}

void main()
{

    vec3 normal = texture(normalMap, fragData.TexCoords).rgb;
    normal      = normalize(normal * 2.0 - 1.0);
    normal      = normalize(fragData.TBN * normal);
    // vec3 albedo = texture(albedo, fragData.TexCoords).rgb;
    vec3  albedo    = pow(texture(albedoMap, fragData.TexCoords).rgb, vec3(2.2));
    float metalness = texture(metalnessMap, fragData.TexCoords).r;
    float roughness = texture(roughnessMap, fragData.TexCoords).r;
    float ao        = texture(aoMap, fragData.TexCoords).r;

    vec3 viewDir = normalize(viewPos - fragData.FragPos);

    vec3 F0 = vec3(0.04);
    F0      = mix(F0, albedo, metalness);

    // reflectance equation
    vec3 Lo = vec3(0.0);

    for (int i = 0; i < numSunLights; i++)
    {

        Lo += CalculateSunLighting(i, viewDir, normal, albedo, metalness, roughness, F0);
    }

    vec3 ambient = ambientLight * albedo * ao;
    vec3 color   = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
    // FragColor = vec4(vec3(CalculateShadow(sunLights[0].shadowBias, max(dot(normal, sunLights[0].direction), 0.0), 0)), 1.0);
}
