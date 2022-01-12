#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Quad.hpp"
#include "Shader.hpp"

struct ShaderProgramLookup
{
    ShaderProgram                      id;
    std::function<void(ShaderProgram)> init;
};

// Todo awful naming cmon man
struct ShaderLookup
{
    std::vector<ShaderStage>         shaderStages;
    std::vector<ShaderProgramLookup> linkedShaderPrograms;
};


class ResourceManager
{
  public:
    ResourceManager()  = default;
    ~ResourceManager() = default;
    static auto& GetInstance()
    {
        static ResourceManager instance;
        return instance;
    }

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    void Initialize();

    void AddShader(ShaderProgram shaderProgram, const std::filesystem::path& path, const std::vector<ShaderStage>& shaderStage,
                   const std::function<void(ShaderProgram)>& initFunction);

    void AddDirtyShader(const std::filesystem::path& path);
    void CheckDirtyShaders();

    inline ShaderProgram GetSkyboxShader() const { return m_SkyboxShader; }
    inline ShaderProgram GetBillboardShader() const { return m_BillboardShader; }
    inline ShaderProgram GetBPForwardLitShader() const { return m_BP_ForwardLitShader; }

    inline ShaderProgram GetBP_DS_LightingShader() const { return m_BP_DS_LightingShader; }
    inline ShaderProgram GetBP_DS_GeometryShader() const { return m_BP_DS_GeometryShader; }

    inline ShaderProgram GetPBR_DS_GeometryShader() const { return m_PBR_DS_GeometryShader; }
    inline ShaderProgram GetPBR_DS_LightingShader() const { return m_PBR_DS_LightingShader; }

  private:
    std::unordered_map<std::string, ShaderLookup> m_ShaderStages;
    std::vector<ShaderLookup>                     m_DirtyShaderStages;
    ShaderProgram                                 m_SkyboxShader;
    ShaderProgram                                 m_BillboardShader;
    ShaderProgram                                 m_BP_ForwardLitShader;
    ShaderProgram                                 m_BP_DS_GeometryShader;
    ShaderProgram                                 m_BP_DS_LightingShader;
    ShaderProgram                                 m_PBR_DS_GeometryShader;
    ShaderProgram                                 m_PBR_DS_LightingShader;
    ShaderStageId                                 m_FallbackFragmentStage;
    Quad                                          m_ScreenQuad;
};