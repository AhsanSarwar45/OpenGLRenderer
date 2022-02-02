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

    inline ShaderProgram GetForwardSunShader() const { return m_ForwardSunShader; }

    inline ShaderProgram GetDSGeometryShader() const { return m_DSGeometryShader; }
    inline ShaderProgram GetDSPointLightShader() const { return m_DSPointLightShader; }
    inline ShaderProgram GetDSSunLightShader() const { return m_DSSunLightShader; }
    inline ShaderProgram GetDSAmbientShader() const { return m_DSAmbientShader; }

    inline Quad GetScreenQuad() const { return m_ScreenQuad; }

    inline ShaderProgram GetSunShadowShader() const { return m_SunShadowShader; }
    inline ShaderProgram GetPointShadowShader() const { return m_PointShadowShader; }

    inline const char* GetOpenGLVersion() const { return m_OpenGLVersion; }

    inline std::shared_ptr<Billboard> GetBillboard() const { return m_Billboard; }

    inline TextureId GetSunLightIcon() const { return m_SunLightIcon; }

    inline TextureId GetPointLightIcon() const { return m_PointLightIcon; }

    std::filesystem::path GetRootPath() const { return m_RootPath; }

  private:
    std::filesystem::path m_RootPath = "../..";

    std::unordered_map<std::string, ShaderLookup> m_ShaderStages;
    std::vector<ShaderLookup>                     m_DirtyShaderStages;
    ShaderProgram                                 m_SkyboxShader;
    ShaderProgram                                 m_BillboardShader;

    ShaderProgram m_ForwardSunShader;

    ShaderProgram m_SunShadowShader;
    ShaderProgram m_PointShadowShader;

    ShaderProgram m_DSGeometryShader;
    ShaderProgram m_DSPointLightShader;
    ShaderProgram m_DSSunLightShader;
    ShaderProgram m_DSAmbientShader;

    const char* m_OpenGLVersion;

    // ShaderStageId m_FallbackFragmentStage;

    TextureId m_PointLightIcon;
    TextureId m_SunLightIcon;

    Quad m_ScreenQuad;

    std::shared_ptr<Billboard> m_Billboard;
};