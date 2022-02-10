#pragma once

#include <memory>

#include "Aliases.hpp"
#include "Billboard.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Model.hpp"
#include "Skybox.hpp"

struct Scene
{
    std::vector<std::shared_ptr<Model>>     models;
    std::vector<std::shared_ptr<Billboard>> billboards;

    std::vector<PointLight> pointLights;
    std::vector<SunLight>   sunLights;

    int sunLightShadowSeperatorIndex;
    int pointLightShadowSeperatorIndex;

    Camera    camera;
    glm::vec3 ambientLight;
    Skybox    skybox;
};

void SetSceneUniforms(const std::shared_ptr<const Scene> scene, ShaderProgram shaderProgram);
void SetSunLightShadow(const std::shared_ptr<Scene> scene, int index, bool value);
void SetPointLightShadow(int index, bool value);
