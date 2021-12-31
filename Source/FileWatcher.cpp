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

        std::filesystem::path filepath = std::filesystem::path(dir + filename);

        filepath.make_preferred();

        ResourceManager::GetInstance().AddDirtyShader(filepath);
        break;
    }
}