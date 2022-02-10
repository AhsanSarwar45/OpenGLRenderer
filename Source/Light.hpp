#pragma once

#include "glm/fwd.hpp"
#include <glm/glm.hpp>

#include "Aliases.hpp"
#include "Camera.hpp"
#include "Frustum.hpp"

struct ShadowRenderData;

struct PointLight
{
    glm::vec3 position;

    glm::vec3 color;

    float power;

    bool castShadows;

    float shadowBias     = 0.08f;
    float shadowNearClip = 0.1f;
    float shadowFarClip  = 20.0f;
};

struct SunLight
{
    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 color;

    float power;

    bool castShadows;

    float shadowBias = 0.2f;

    float zMult = 2.5f;
};

struct SunLightUniformData
{
    glm::vec4 position;
    glm::vec4 direction;

    glm::vec4 color;

    float power;
    float shadowBias;
    float PADDING1;
    float PADDING2;
};

struct PointLightUniformData
{
    glm::vec4 position;

    glm::vec4 color;

    float power;

    float shadowBias;
    float shadowNearClip;
    float shadowFarClip;
};
struct LightTransform
{
    glm::mat4 LightSpaceVPMatrix; // light's shadow rendering view-projection matrix
};

void SetSunLightUniform(ShaderProgram shaderProgram, const SunLight& sunLight);

glm::mat4 GetSunLightMatrix(const SunLight& sunLight, const Camera& camera, float nearPlane, float farPlane);

std::vector<glm::mat4> GetSunLightMatrices(const SunLight& sunLight, const Camera& camera, ShadowRenderData& shadowData);
