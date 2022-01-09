#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct PointLight
{
    vec3 position;

    vec3 color;

    float power;

    float linear;
    float quadratic;
};

const int MAX_LIGHTS = 32;

uniform PointLight pointLights[MAX_LIGHTS];
uniform int        numPointLights;

uniform vec3 viewPos;

uniform vec3 ambientLight;

void main()
{
    // retrieve data from gbuffer
    vec3  fragPos  = texture(gPosition, TexCoords).rgb;
    vec3  normal   = texture(gNormal, TexCoords).rgb;
    vec3  albedo   = texture(gAlbedoSpec, TexCoords).rgb;
    float specular = texture(gAlbedoSpec, TexCoords).a;

    // then calculate lighting as usual
    vec3 lighting = (albedo + specular) * ambientLight; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - fragPos);
    for (int i = 0; i < numPointLights; ++i)
    {
        // diffuse
        vec3 lightDir = normalize(pointLights[i].position - fragPos);
        vec3 diffuse  = max(dot(normal, lightDir), 0.0) * albedo;
        // specular
        vec3  halfwayDir = normalize(lightDir + viewDir);
        float spec       = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
        spec *= specular;
        // attenuation
        float distance = length(pointLights[i].position - fragPos);
        // float attenuation =
        //     1.0 / (1.0 + pointLights[i].linear * distance + pointLights[i].quadratic * distance * distance);
        float attenuation = 1.0 / (distance * distance);
        diffuse *= attenuation;
        spec *= attenuation;
        lighting += (diffuse + spec) * pointLights[i].color * (pointLights[i].power / 5.0);
    }
    FragColor = vec4(lighting, 1.0);
}