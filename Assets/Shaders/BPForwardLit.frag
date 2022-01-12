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
    sampler2D specular;
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

const int MAX_LIGHTS = 32;

uniform PointLight pointLights[MAX_LIGHTS];
uniform int        numPointLights;

uniform vec3 viewPos;

uniform vec3 ambientLight;

void main()
{

    vec3 normal = texture(material.normal, fragData.TexCoords).rgb;
    normal      = normalize(normal * 2.0 - 1.0);
    normal      = normalize(fragData.TBN * normal);
    // vec3 albedo = texture(material.albedo, fragData.TexCoords).rgb;
    vec3  albedo   = pow(texture(material.albedo, fragData.TexCoords).rgb, vec3(2.2));
    float specular = texture(material.specular, fragData.TexCoords).r;

    // then calculate lighting as usual
    vec3 lighting = (albedo + specular) * ambientLight; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - fragData.FragPos);
    for (int i = 0; i < numPointLights; ++i)
    {
        // diffuse
        vec3 lightDir = normalize(pointLights[i].position - fragData.FragPos);
        vec3 diffuse  = max(dot(normal, lightDir), 0.0) * albedo;
        // specular
        vec3  halfwayDir = normalize(lightDir + viewDir);
        float spec       = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
        spec *= specular;
        // attenuation
        float distance = length(pointLights[i].position - fragData.FragPos);
        // float attenuation =
        //     1.0 / (1.0 + pointLights[i].linear * distance + pointLights[i].quadratic * distance * distance);
        float attenuation = 1.0 / (distance * distance);
        diffuse *= attenuation;
        spec *= attenuation;
        lighting += (diffuse + spec) * pointLights[i].color * (pointLights[i].power / 4.0);
    }
    FragColor = vec4(lighting, 1.0);
}
