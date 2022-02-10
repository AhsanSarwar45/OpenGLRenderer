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
        ShaderSetFloat(shaderProgram, "pointLights[" + indexStr + "].shadowBias", pointLight.shadowBias);
        ShaderSetFloat(shaderProgram, "pointLights[" + indexStr + "].shadowFarClip", pointLight.shadowFarClip);

        index++;
    }
    index = 0;
    for (const auto& sunLight : scene->sunLights)
    {
        std::string indexStr = std::to_string(index);
        ShaderSetFloat3(shaderProgram, "sunLights[" + indexStr + "].direction", sunLight.direction);
        ShaderSetFloat3(shaderProgram, "sunLights[" + indexStr + "].color", sunLight.color);
        ShaderSetFloat(shaderProgram, "sunLights[" + indexStr + "].power", sunLight.power);
        ShaderSetFloat(shaderProgram, "sunLights[" + indexStr + "].shadowBias", sunLight.shadowBias);

        index++;
    }
}
void SetSunLightShadow(const std::shared_ptr<Scene> scene, int index, bool value)
{
    bool isShadowOn = index < scene->sunLightShadowSeperatorIndex;
    if (isShadowOn != value)
    {
        int initialIndex, finalIndex;

        if (value)
        {
            initialIndex = index;
            // finalIndex   = scene->sunLights.length;
        }
        else
        {
            initialIndex = scene->sunLightShadowSeperatorIndex + index;
        }

        SunLight light = scene->sunLights[index];
        scene->sunLights.erase(scene->sunLights.begin() + index);
    }
}
