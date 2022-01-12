#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Quad.hpp"
#include "Shader.hpp"

// Todo awful naming cmon man
struct ShaderLookup
{
    ShaderStage                shaderStage;
    std::vector<ShaderProgram> linkedShaderPrograms;
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

    void AddShader(ShaderProgram shaderProgram, ShaderStage shaderStage);

    void AddDirtyShader(const std::filesystem::path& path);
    void CheckDirtyShaders();

    inline ShaderProgram GetSkyboxShader() const { return m_SkyboxShader; }
    inline ShaderProgram GetBillboardShader() const { return m_BillboardShader; }
    inline ShaderProgram GetBPForwardLitShader() const { return m_BPForwardLitShader; }

  private:
    std::unordered_map<std::string, ShaderLookup> m_ShaderStages;
    std::vector<ShaderLookup>                     m_DirtyShaderStages;
    ShaderProgram                                 m_SkyboxShader;
    ShaderProgram                                 m_BillboardShader;
    ShaderProgram                                 m_BPForwardLitShader;
    ShaderStageId                                 m_FallbackFragmentStage;
    Quad                                          m_ScreenQuad;
};