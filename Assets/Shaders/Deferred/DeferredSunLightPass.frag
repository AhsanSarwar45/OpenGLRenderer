#version 430 core
out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetalnessRoughnessAO;

layout(binding = 12) uniform sampler2DArray sunShadowMapArray;

struct SunLight
{
    vec4  position;
    vec4  direction;
    vec4  color;
    float power;
    float shadowBias;
};

layout(std140, binding = 2) uniform SunLightTransform { mat4 lightSpaceVPMatrix[600]; }
sunLightTransform;
layout(std140, binding = 3) uniform SunLightArray { SunLight sunLights[100]; }
sunLightArray; // todo rename to block

layout(location = 0) in vec2 texCoord;
layout(location = 1) flat in int lightIndex;

const float PI = 3.14159265359;

uniform vec3 viewPos;

vec3 fresnelSchlick(float cosTheta, vec3 F0) { return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0); }

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
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

float CalculateSunShadow(float shadowBias, vec3 fragPos, float dotLightNormal)
{
    vec4 fragPosLightSpace = sunLightTransform.lightSpaceVPMatrix[lightIndex] * vec4(fragPos, 1.0);
    vec3 projCoords        = (fragPosLightSpace.xyz / fragPosLightSpace.w) * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    if (currentDepth > 1.0)
    {
        currentDepth = 1.0;
    }

    vec3 shadowMapTexelCoord = vec3(projCoords.x, projCoords.y, lightIndex);

    float closestDepth = texture(sunShadowMapArray, shadowMapTexelCoord).r;

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

vec3 CalculateSunLighting(vec3 viewDir, vec3 normal, vec3 albedo, float metallness, float roughness, vec3 F0, vec3 fragPos)
{
    SunLight sunLight = sunLightArray.sunLights[lightIndex];

    vec3 lightDir = normalize(sunLight.direction.xyz);

    vec3 halfwayDir = normalize(viewDir + lightDir);
    vec3 radiance   = sunLight.color.xyz * sunLight.power;

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

    float shadow   = CalculateSunShadow(sunLight.shadowBias, fragPos, NdotL);
    vec3  lighting = (kD * albedo / PI + specular) * radiance * NdotL;

    return lighting * (1 - shadow);
}

void main()
{
    // retrieve data from gbuffer
    vec3 fragPos = texture(gPosition, texCoord).rgb;

    vec3  albedo               = texture(gAlbedo, texCoord).rgb;
    vec3  normal               = texture(gNormal, texCoord).rgb;
    vec3  metalnessRoughnessAO = texture(gMetalnessRoughnessAO, texCoord).rgb;
    float metalness            = metalnessRoughnessAO.r;
    float roughness            = metalnessRoughnessAO.g;

    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 F0 = vec3(0.04);
    F0      = mix(F0, albedo, metalness);

    // reflectance equation

    vec3 color = CalculateSunLighting(viewDir, normal, albedo, metalness, roughness, F0, fragPos);

    // color = vec3(metalness);

    FragColor = vec4(color, 1.0);
}