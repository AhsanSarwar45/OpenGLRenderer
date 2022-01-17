#include "FileWatcher.hpp"

#include <iostream>
#include <string>

#include <glad/glad.h>

#include "ResourceManager.hpp"
#include "Shader.hpp"

ShaderUpdateListener::ShaderUpdateListener(float minFileModifyTime) : m_MinFileModifyTime(minFileModifyTime) {}

void ShaderUpdateListener::handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action,
                                            std::string oldFilename)
{

    switch (action)
    {
    case efsw::Actions::Modified: //! gets called twice

        std::filesystem::path filepath = std::filesystem::path(dir) / std::filesystem::path(filename);

        filepath.make_preferred();

        std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();

        if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_LastModifyTimes[filepath.string()]).count() >
            m_MinFileModifyTime)
        {
            m_LastModifyTimes[filepath.string()] = currentTime;
            printf("%s", "changed!");
            ResourceManager::GetInstance().AddDirtyShader(filepath);
        }
        break;
    }
}