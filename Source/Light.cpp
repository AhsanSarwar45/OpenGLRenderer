#include "Light.hpp"

#include "Render.hpp"
#include "Uniform.hpp"

void SetSunLightUniform(ShaderProgram shaderProgram, const SunLight& sunLight)
{
    ShaderSetFloat3(shaderProgram, "sunLight.direction", sunLight.direction);
    ShaderSetFloat3(shaderProgram, "sunLight.color", sunLight.color);
    ShaderSetFloat(shaderProgram, "sunLight.power", sunLight.power);
}

glm::mat4 GetSunLightMatrix(const SunLight& sunLight, const Camera& camera, float nearPlane, float farPlane)
{
    ViewProjection viewProjection = {.projectionMatrix = glm::perspective(glm::radians(camera.zoom),
                                                                          (float)camera.viewportWidth / (float)camera.viewportHeight,
                                                                          camera.farClip * nearPlane, camera.farClip * farPlane),
                                     .viewMatrix       = camera.viewProjection.viewMatrix};

    const std::vector<glm::vec4> corners = GetFrustumCornersWorldSpace(viewProjection);

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();

    const auto lightView = glm::lookAt(center + sunLight.direction, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::min();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::min();
    for (const auto& v : corners)
    {
        const auto trf = lightView * v;
        minX           = std::min(minX, trf.x);
        maxX           = std::max(maxX, trf.x);
        minY           = std::min(minY, trf.y);
        maxY           = std::max(maxY, trf.y);
        minZ           = std::min(minZ, trf.z);
        maxZ           = std::max(maxZ, trf.z);
    }

    if (minZ < 0)
    {
        minZ *= sunLight.zMult;
    }
    else
    {
        minZ /= sunLight.zMult;
    }
    if (maxZ < 0)
    {
        maxZ /= sunLight.zMult;
    }
    else
    {
        maxZ *= sunLight.zMult;
    }

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

    return lightProjection * lightView;
}

std::vector<glm::mat4> GetSunLightMatrices(const SunLight& sunLight, const Camera& camera, ShadowRenderData& shadowData)
{
    std::vector<glm::mat4> sunLightMatrices;

    for (size_t i = 0; i < shadowData.numShadowCascades + 1; ++i)
    {
        float nearClip;
        float farClip;

        if (i == 0)
        {
            nearClip = camera.nearClip / camera.farClip;
            farClip  = shadowData.shadowCascadeLevels[i];
        }
        else if (i < shadowData.numShadowCascades)
        {
            nearClip = shadowData.shadowCascadeLevels[i - 1];
            farClip  = shadowData.shadowCascadeLevels[i];
        }
        else
        {
            nearClip = shadowData.shadowCascadeLevels[i - 1];
            farClip  = 1;
        }
        sunLightMatrices.push_back(GetSunLightMatrix(sunLight, camera, nearClip, farClip));
        shadowData.shadowCascadeDistances[i] = camera.farClip * farClip;
    }
    return sunLightMatrices;
}
