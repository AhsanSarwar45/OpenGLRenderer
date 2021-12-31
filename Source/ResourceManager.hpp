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

    inline void AddDirtyShader(const std::filesystem::path& path)
    {
        std::string name = path.string();
        auto        it   = m_ShaderStages.find(name);

        if (it != m_ShaderStages.end())
        {
            m_DirtyShaderStages.push_back(it->second);
        }
    }
    inline void CheckDirtyShaders()
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

    inline void AddShader(ShaderProgram shaderProgram, ShaderStage shaderStage)
    {
        m_ShaderStages[shaderStage.path.string()].shaderStage = shaderStage;
        m_ShaderStages[shaderStage.path.string()].linkedShaderPrograms.push_back(shaderProgram);
    }

  private:
    std::unordered_map<std::string, ShaderLookup> m_ShaderStages;
    std::vector<ShaderLookup>                     m_DirtyShaderStages;
};