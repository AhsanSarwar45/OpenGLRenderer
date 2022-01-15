#include "ResourceManager.hpp"
#include "Billboard.hpp"
#include "Quad.hpp"
#include "Shader.hpp"
#include <memory>
#include <vector>

void ResourceManager::Initialize()
{
    m_SkyboxShader    = LoadShader({"../Assets/Shaders/Skybox.vert", "../Assets/Shaders/Skybox.frag"}, "Skybox", false);
    m_BillboardShader = LoadShader({"../Assets/Shaders/Billboard.vert", "../Assets/Shaders/Billboard.frag"}, "Billboard");

    m_BP_ForwardLitShader  = LoadShader({"../Assets/Shaders/ForwardLit.vert", "../Assets/Shaders/BPForwardLit.frag"}, "BP Forward");
    m_PBR_ForwardLitShader = LoadShader({"../Assets/Shaders/ForwardLit.vert", "../Assets/Shaders/PBRForwardLit.frag"}, "PBR Forward");

    m_ShadowShader = LoadShader(
        {"../Assets/Shaders/DepthPass.vert", "../Assets/Shaders/DepthPass.frag", "../Assets/Shaders/DepthPass.geom"}, "PBR Forward", false);

    m_PBR_DS_GeometryShader =
        LoadShader({"../Assets/Shaders/DeferredGeometryPass.vert", "../Assets/Shaders/PBRDeferredGeometryPass.frag"}, "PBR Geometry Pass");
    m_PBR_DS_LightingShader =
        LoadShader({"../Assets/Shaders/DeferredLightPass.vert", "../Assets/Shaders/PBRDeferredLightPass.frag"}, "PBR Light Pass", false);

    m_BP_DS_GeometryShader =
        LoadShader({"../Assets/Shaders/DeferredGeometryPass.vert", "../Assets/Shaders/BPDeferredGeometryPass.frag"}, "BP Geometry Pass");
    m_BP_DS_LightingShader =
        LoadShader({"../Assets/Shaders/DeferredLightPass.vert", "../Assets/Shaders/BPDeferredLightPass.frag"}, "BP Light Pass", false);

    m_ScreenQuad = CreateQuad();

    m_LightBillboard        = std::make_shared<Billboard>();
    *m_LightBillboard       = LoadBillboard("../Assets/Images/BulbIcon2.png");
    m_LightBillboard->scale = {0.5f, 0.5f};
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
    if (m_DirtyShaderStages.empty())
    {
        return;
    }

    for (auto& dirtShader : m_DirtyShaderStages)
    {
        for (auto& shaderProgram : dirtShader.linkedShaderPrograms)
        {
            ShaderInternal::LoadShaderStages(shaderProgram.id, dirtShader.shaderStages);
            if (shaderProgram.init)
            {
                shaderProgram.init(shaderProgram.id);
            }
        }
    }
    m_DirtyShaderStages.clear();
}

// TODO: Very unclear
void ResourceManager::AddShader(ShaderProgram shaderProgram, const std::filesystem::path& path, const std::vector<ShaderStage>& shaderStage,
                                const std::function<void(ShaderProgram)>& initFunction)
{
    m_ShaderStages[path.string()].shaderStages = shaderStage;
    m_ShaderStages[path.string()].linkedShaderPrograms.push_back({shaderProgram, initFunction});
}