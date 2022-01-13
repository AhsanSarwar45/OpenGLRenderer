#version 330 core
out vec4 FragColor;

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

struct PointLight
{
    vec3 position;

    vec3 color;

    float power;

    float linear;
    float quadratic;
};

uniform Material material;

const float PI         = 3.14159265359;
const int   MAX_LIGHTS = 32;

uniform PointLight pointLights[MAX_LIGHTS];
uniform int        numPointLights;

uniform vec3 viewPos;

uniform vec3 ambientLight;

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

void main()
{

    vec3 normal = texture(material.normal, fragData.TexCoords).rgb;
    normal      = normalize(normal * 2.0 - 1.0);
    normal      = normalize(fragData.TBN * normal);
    // vec3 albedo = texture(material.albedo, fragData.TexCoords).rgb;
    vec3  albedo    = pow(texture(material.albedo, fragData.TexCoords).rgb, vec3(2.2));
    float metallic  = texture(material.metalness, fragData.TexCoords).r;
    float roughness = texture(material.roughness, fragData.TexCoords).r;
    float ao        = texture(material.ao, fragData.TexCoords).r;

    vec3 fragPos = fragData.FragPos;

    vec3 N = normalize(normal);
    vec3 V = normalize(viewPos - fragPos);

    vec3 F0 = vec3(0.04);
    F0      = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < numPointLights; ++i)
    {
        // calculate per-light radiance
        vec3  L           = normalize(pointLights[i].position - fragPos);
        vec3  H           = normalize(V + L);
        float distance    = length(pointLights[i].position - fragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3  radiance    = pointLights[i].color * attenuation * pointLights[i].power;

        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        vec3  numerator   = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3  specular    = numerator / denominator;

        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = ambientLight * albedo * ao;
    vec3 color   = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
