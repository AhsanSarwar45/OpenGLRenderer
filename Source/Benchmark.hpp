#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <vector>

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
    bool                                  isRunning;
    std::vector<BenchmarkResult>          results;
    float                                 duration;
    std::vector<const char*>              renderingPipelines;
    int                                   numPipelines;
};

int RunBenchmark(BenchmarkData& benchmarkData, Window& window, std::function<void(int)> setRenderPipeline);