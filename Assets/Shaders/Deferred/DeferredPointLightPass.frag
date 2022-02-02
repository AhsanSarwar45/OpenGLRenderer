#version 430 core
out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetalnessRoughnessAO;

layout(binding = 13) uniform samplerCubeArray pointShadowMapArray;

struct PointLight
{
    vec4 position;

    vec4 color;

    float power;

    float shadowBias;
    float shadowNearClip;
    float shadowFarClip;
};

layout(std140, binding = 4) uniform PointLightTransform { mat4 lightSpaceVPMatrix[600]; }
pointLightTransform;
layout(std140, binding = 5) uniform PointLightArray { PointLight pointLights[100]; }
pointLightArray;

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

float CalculatePointShadow(float shadowBias, vec3 fragPos, float dotLightNormal)
{

    vec3 fragToLight = fragPos - pointLightArray.pointLights[lightIndex].position.xyz;

    float closestDepth = texture(pointShadowMapArray, vec4(fragToLight, lightIndex)).r;

    closestDepth *= pointLightArray.pointLights[lightIndex].shadowFarClip;

    float currentDepth = length(fragToLight);

    float bias = max(shadowBias * (1.0 - dotLightNormal), shadowBias / 10.0);

    float shadow = currentDepth > (closestDepth + bias) ? 1 : 0.0;

    return shadow;
}

vec3 CalculatePointLighting(vec3 viewDir, vec3 normal, vec3 albedo, float metallness, float roughness, vec3 F0, vec3 fragPos)
{

    PointLight pointLight = pointLightArray.pointLights[lightIndex];

    vec3 lightDir = normalize(pointLight.position.xyz - fragPos);

    vec3 halfwayDir = normalize(viewDir + lightDir);

    float distance    = length(pointLight.position.xyz - fragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3  radiance    = pointLight.color.xyz * pointLight.power * attenuation;

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

    float shadow   = CalculatePointShadow(pointLight.shadowBias, fragPos, NdotL);
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

    vec3 color = CalculatePointLighting(viewDir, normal, albedo, metalness, roughness, F0, fragPos);

    FragColor = vec4(color, 1.0);
}