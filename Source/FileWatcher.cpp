#include "FileWatcher.hpp"

#include <iostream>
#include <string>

#include <glad/glad.h>

#include "ResourceManager.hpp"
#include "Shader.hpp"

// ShaderUpdateListener::ShaderUpdateListener(std::unordered_map<std::string, Shader*>* shaders) : m_Shaders(shaders) {}

void ShaderUpdateListener::handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename,
                                            efsw::Action action, std::string oldFilename)
{

    switch (action)
    {
    case efsw::Actions::Modified:

        std::string::size_type idx = filename.rfind('.');

        std::string extension;

        if (idx != std::string::npos)
        {
            extension                         = filename.substr(idx + 1);
            std::filesystem::path filepath    = std::filesystem::path(dir + filename);
            std::filesystem::path oldFilepath = std::filesystem::path(dir + oldFilename);

            filepath.make_preferred();
            oldFilepath.make_preferred();

            ResourceManager::GetInstance().AddDirtyShader(filepath);
        }

        // = LoadShader(const std::string &vertexShaderPath, const std::string
        // &fragmentShaderPath, const char *name);
        break;
    }
}