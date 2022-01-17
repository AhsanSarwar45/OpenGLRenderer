#pragma once

#include <chrono>
#include <string>
#include <unordered_map>

#include <efsw/include/efsw/efsw.hpp>

#include "Shader.hpp"

class ShaderUpdateListener : public efsw::FileWatchListener
{
  public:
    ShaderUpdateListener(float minFileModifyTime = 0.5f);
    void handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action,
                          std::string oldFilename) override;

  private:
    // std::unordered_map<std::string, Shader*>* m_Shaders;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> m_LastModifyTimes;
    float                                                                  m_MinFileModifyTime;
};
