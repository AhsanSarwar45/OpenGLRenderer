#pragma once

#include <memory>

#include "Camera.hpp"
#include "Light.hpp"
#include "Model.hpp"
#include "Skybox.hpp"

struct Scene
{
    std::vector<std::shared_ptr<Model>> models;
    std::vector<PointLight>             pointLights;
    std::shared_ptr<Camera>             camera;
    glm::vec3                           ambientLight;
    Skybox                              skybox;
};
