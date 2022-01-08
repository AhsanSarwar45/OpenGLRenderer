#pragma once

#include <memory>

#include "Light.hpp"
#include "Model.hpp"
#include "Skybox.hpp"

struct Scene
{
    std::vector<std::shared_ptr<Model>> models;
    std::vector<PointLight>             pointLights;

    glm::vec3 ambientLight;

    Skybox skybox;
};
