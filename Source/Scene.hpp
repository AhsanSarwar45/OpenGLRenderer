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
    std::vector<PointLight>                 pointLights;
    std::vector<SunLight>                   sunLights;
    Camera                                  camera;
    glm::vec3                               ambientLight;
    Skybox                                  skybox;
};

void SetSceneUniforms(const std::shared_ptr<const Scene> scene, ShaderProgram shaderProgram);
