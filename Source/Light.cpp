#include "Light.hpp"

#include "Uniform.hpp"

void SetSunLightUniform(ShaderProgram shaderProgram, const SunLight& sunLight)
{
    ShaderSetFloat3(shaderProgram, "sunLight.direction", sunLight.direction);
    ShaderSetFloat3(shaderProgram, "sunLight.color", sunLight.color);
    ShaderSetFloat(shaderProgram, "sunLight.power", sunLight.power);
}
