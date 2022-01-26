#pragma once

#include "Aliases.hpp"
#include "glm/fwd.hpp"
#include <glm/glm.hpp>

struct PointLight
{
    glm::vec3 position;

    glm::vec3 color;

    float power;
};

struct SunLight
{
    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 color;

    float power;

    float shadowBias     = 0.05f;
    float shadowNearClip = 0.1f;
    float shadowFarClip  = 20.0f;
    float shadowMapOrtho = 10.0f;
};

struct LightTransform
{
    glm::mat4 LightSpaceVPMatrix; // light's shadow rendering view-projection matrix
};

void SetSunLightUniform(ShaderProgram shaderProgram, const SunLight& sunLight);
