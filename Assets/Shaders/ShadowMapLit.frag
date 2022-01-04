#version 330 core
out vec4 FragColor;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
}
fs_in;

struct Material
{
    sampler2D diffuse;
    sampler2D normal;
    sampler2D specular;
    sampler2D roughness;
};

uniform sampler2D shadowMap;

struct Light
{
    vec3 direction;
    vec3 position;

    vec3 ambient;
    vec3 specular;
    vec3 diffuse;

    float constant;
    float linear;
    float quadratic;
};

uniform vec3     viewPos;
uniform Material material;
uniform Light    light;

float ShadowCalculation(vec4 fragPosLightSpace, float dotLightNormal)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    if (currentDepth > 1.0)
    {
        currentDepth = 1.0;
    }
    float bias = max(0.01 * (1.0 - dotLightNormal), 0.005);
    // check whether current frag pos is in shadow
    float shadow    = 0.0;
    vec2  texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main()
{
    vec3 color  = texture(material.diffuse, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);

    // ambient
    vec3 ambient = light.ambient;
    // diffuse
    // vec3  lightDir       = normalize(light.position - fs_in.FragPos);
    vec3  lightDir       = normalize(light.direction);
    float dotLightNormal = dot(lightDir, normal);
    float diff           = max(dotLightNormal, 0.0);
    vec3  diffuse        = diff * light.diffuse;
    // specular
    vec3  viewDir    = normalize(viewPos - fs_in.FragPos);
    vec3  reflectDir = reflect(-lightDir, normal);
    float spec       = 0.0;
    vec3  halfwayDir = normalize(lightDir + viewDir);
    spec             = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular    = spec * light.specular;

    // calculate shadow
    float shadow   = ShadowCalculation(fs_in.FragPosLightSpace, dotLightNormal);
    vec3  lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);
}