#include "ResourceManager.hpp"

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