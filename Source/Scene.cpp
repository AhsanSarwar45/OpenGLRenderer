#include "Scene.hpp"

void SetSceneUniforms(const std::shared_ptr<const Scene> scene, ShaderProgram shaderProgram)
{

    UseShaderProgram(shaderProgram);

    ShaderSetInt(shaderProgram, "numPointLights", scene->pointLights.size());
    ShaderSetInt(shaderProgram, "numSunLights", scene->sunLights.size());
    ShaderSetFloat3(shaderProgram, "ambientLight", scene->ambientLight);
    // send light relevant uniforms
    int index = 0;
    for (const auto& pointLight : scene->pointLights)
    {
        std::string indexStr = std::to_string(index);
        ShaderSetFloat3(shaderProgram, "pointLights[" + indexStr + "].position", pointLight.position);
        ShaderSetFloat3(shaderProgram, "pointLights[" + indexStr + "].color", pointLight.color);
        ShaderSetFloat(shaderProgram, "pointLights[" + indexStr + "].power", pointLight.power);
        ShaderSetFloat(shaderProgram, "pointLights[" + indexStr + "].linear", pointLight.linear);
        ShaderSetFloat(shaderProgram, "pointLights[" + indexStr + "].quadratic", pointLight.quadratic);

        index++;
    }
    index = 0;
    for (const auto& sunLight : scene->sunLights)
    {
        std::string indexStr = std::to_string(index);
        ShaderSetFloat3(shaderProgram, "sunLights[" + indexStr + "].direction", sunLight.direction);
        ShaderSetFloat3(shaderProgram, "sunLights[" + indexStr + "].color", sunLight.color);
        ShaderSetFloat(shaderProgram, "sunLights[" + indexStr + "].power", sunLight.power);

        index++;
    }

    ShaderSetFloat3(shaderProgram, "viewPos", scene->camera->GetPosition());
}