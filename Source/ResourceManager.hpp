#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Billboard.hpp"
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

    inline ShaderProgram GetForwardLitShader() const { return m_ForwardLitShader; }

    inline ShaderProgram GetDSGeometryShader() const { return m_DSGeometryShader; }
    inline ShaderProgram GetDSLightingShader() const { return m_DSLightingShader; }

    inline Quad GetScreenQuad() const { return m_ScreenQuad; }

    inline ShaderProgram GetShadowShader() const { return m_ShadowShader; }

    inline const char* GetOpenGLVersion() const { return m_OpenGLVersion; }

    inline std::shared_ptr<Billboard> GetBillboard() const { return m_Billboard; }

    inline TextureId GetSunLightIcon() const { return m_SunLightIcon; }

    inline TextureId GetPointLightIcon() const { return m_PointLightIcon; }

  private:
    std::unordered_map<std::string, ShaderLookup> m_ShaderStages;
    std::vector<ShaderLookup>                     m_DirtyShaderStages;
    ShaderProgram                                 m_SkyboxShader;
    ShaderProgram                                 m_BillboardShader;

    ShaderProgram m_ForwardLitShader;
    ShaderProgram m_ShadowShader;

    ShaderProgram m_DSGeometryShader;
    ShaderProgram m_DSLightingShader;

    const char* m_OpenGLVersion;

    // ShaderStageId m_FallbackFragmentStage;

    TextureId m_PointLightIcon;
    TextureId m_SunLightIcon;

    Quad m_ScreenQuad;

    std::shared_ptr<Billboard> m_Billboard;
};