#include "ResourceManager.hpp"
#include "Quad.hpp"
#include "Shader.hpp"

void ResourceManager::Initialize()
{
    m_SkyboxShader = LoadShader("../Assets/Shaders/Skybox.vert", "../Assets/Shaders/Skybox.frag", "Skybox", false);
    // m_BillboardShader =
    //     LoadShader("../Assets/Shaders/ForwardLit.vert", "../Assets/Shaders/Billboard.frag", "Billboard");
    m_BPForwardLitShader =
        LoadShader("../Assets/Shaders/ForwardLit.vert", "../Assets/Shaders/BPForwardLit.frag", "Forward");

    m_ScreenQuad = CreateQuad();
}

void ResourceManager::AddDirtyShader(const std::filesystem::path& path)
{
    std::string name = path.string();
    auto        it   = m_ShaderStages.find(name);

    if (it != m_ShaderStages.end())
    {
        m_DirtyShaderStages.push_back(it->second);
    }
}

void ResourceManager::CheckDirtyShaders()
{
    for (const auto& dirtShader : m_DirtyShaderStages)
    {
        for (const auto& shaderProgram : dirtShader.linkedShaderPrograms)
        {
            ShaderInternal::ReloadShaderStage(shaderProgram, dirtShader.shaderStage);
        }
    }
    m_DirtyShaderStages.clear();
}

void ResourceManager::AddShader(ShaderProgram shaderProgram, ShaderStage shaderStage)
{
    m_ShaderStages[shaderStage.path.string()].shaderStage = shaderStage;
    m_ShaderStages[shaderStage.path.string()].linkedShaderPrograms.push_back(shaderProgram);
}