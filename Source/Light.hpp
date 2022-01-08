#pragma once

#include "glm/fwd.hpp"
#include <glm/glm.hpp>

struct PointLight
{
    glm::vec3 position;

    glm::vec3 color;

    float power;

    float linear;
    float quadratic;
};