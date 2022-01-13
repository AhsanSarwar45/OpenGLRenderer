#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <vector>

#include "Render.hpp"

struct BenchmarkResult
{
    const char* name;

    float totalTime;
    int   frameCount;

    float averageTime;
    float averageFPS;

    float lowestFPS;
    float highestFPS;
};

class Window;

struct BenchmarkData
{
    std::chrono::steady_clock::time_point beginTime;
    std::shared_ptr<DSRenderData>         dsRenderData;
    std::shared_ptr<ForwardRenderData>    forwardRenderData;
    bool                                  isRunning;
    std::vector<BenchmarkResult>          results;
    float                                 duration;
    std::vector<RenderingPipeline>        renderingPipelines;
};

int RunBenchmark(BenchmarkData& benchmarkData, Window& window, std::vector<RenderPass>& renderPasses);