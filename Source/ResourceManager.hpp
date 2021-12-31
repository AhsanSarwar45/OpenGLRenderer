#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

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

    void AddDirtyShader(const std::filesystem::path& path);
    void CheckDirtyShaders();

    void AddShader(ShaderProgram shaderProgram, ShaderStage shaderStage);

  private:
    std::unordered_map<std::string, ShaderLookup> m_ShaderStages;
    std::vector<ShaderLookup>                     m_DirtyShaderStages;
};