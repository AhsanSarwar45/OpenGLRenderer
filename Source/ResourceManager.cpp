#include "ResourceManager.hpp"
#include "Billboard.hpp"
#include "Quad.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include <memory>
#include <vector>

void ResourceManager::Initialize()
{
    m_SkyboxShader = LoadShader({"../Assets/Shaders/Skybox/Skybox.vert", "../Assets/Shaders/Skybox/Skybox.frag"}, "Skybox", false);
    m_BillboardShader =
        LoadShader({"../Assets/Shaders/Billboard/Billboard.vert", "../Assets/Shaders/Billboard/Billboard.frag"}, "Billboard");

    m_ForwardSunShader = LoadShader({"../Assets/Shaders/Forward/ForwardLit.vert", "../Assets/Shaders/Forward/ForwardSun.frag"}, "Forward");

    m_ShadowShader = LoadShader({"../Assets/Shaders/ShadowMap/ShadowPass.vert", "../Assets/Shaders/ShadowMap/ShadowPass.frag",
                                 "../Assets/Shaders/ShadowMap/ShadowPass.geom"},
                                "Shadow", false);

    m_DSGeometryShader = LoadShader(
        {"../Assets/Shaders/Deferred/DeferredGeometryPass.vert", "../Assets/Shaders/Deferred/DeferredGeometryPass.frag"}, "Geometry Pass");
    m_DSLightingShader = LoadShader(
        {"../Assets/Shaders/Deferred/DeferredLightPass.vert", "../Assets/Shaders/Deferred/DeferredLightPass.frag"}, "Light Pass", false);

    m_ScreenQuad = CreateQuad();

    m_PointLightIcon = LoadTexture("../Assets/Images/BulbIcon2.png").id;
    m_SunLightIcon   = LoadTexture("../Assets/Images/sunIcon.png").id;

    m_Billboard        = std::make_shared<Billboard>();
    *m_Billboard       = CreateBillboard();
    m_Billboard->scale = {0.5f, 0.5f};
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