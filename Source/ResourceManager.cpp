#include "ResourceManager.hpp"
#include "Quad.hpp"
#include "Shader.hpp"
#include <vector>

void ResourceManager::Initialize()
{
    m_SkyboxShader = LoadShader({"../Assets/Shaders/Skybox.vert", "../Assets/Shaders/Skybox.frag"}, "Skybox", false);
    // m_BillboardShader =
    //     LoadShader("../Assets/Shaders/ForwardLit.vert", "../Assets/Shaders/Billboard.frag", "Billboard");
    m_BP_ForwardLitShader = LoadShader({"../Assets/Shaders/ForwardLit.vert", "../Assets/Shaders/BPForwardLit.frag"}, "Forward");

    m_PBR_DS_GeometryShader =
        LoadShader({"../Assets/Shaders/DeferredGeometryPass.vert", "../Assets/Shaders/PBRDeferredGeometryPass.frag"}, "Geometry Pass");
    m_PBR_DS_LightingShader =
        LoadShader({"../Assets/Shaders/DeferredLightPass.vert", "../Assets/Shaders/PBRDeferredLightPass.frag"}, "Light Pass", false);

    m_BP_DS_GeometryShader =
        LoadShader({"../Assets/Shaders/DeferredGeometryPass.vert", "../Assets/Shaders/BPDeferredGeometryPass.frag"}, "Geometry Pass");
    m_BP_DS_LightingShader =
        LoadShader({"../Assets/Shaders/DeferredLightPass.vert", "../Assets/Shaders/BPDeferredLightPass.frag"}, "Light Pass", false);

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
    for (auto& dirtShader : m_DirtyShaderStages)
    {
        for (auto& shaderProgram : dirtShader.linkedShaderPrograms)
        {
            ShaderInternal::ReloadShaderStage(shaderProgram, dirtShader.shaderStages);
        }
    }
    m_DirtyShaderStages.clear();
}

// TODO: Very unclear
void ResourceManager::AddShader(ShaderProgram shaderProgram, const std::filesystem::path& path, const std::vector<ShaderStage>& shaderStage)
{
    m_ShaderStages[path.string()].shaderStages = shaderStage;
    m_ShaderStages[path.string()].linkedShaderPrograms.push_back(shaderProgram);
}