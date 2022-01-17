#include "Benchmark.hpp"

#include "Window.hpp"

int RunBenchmark(BenchmarkData& benchmarkData, Window& window, std::vector<RenderPass>& renderPasses)
{

    static int i = 0;

    static int frameCount = 0;

    static float lowestFPS  = 0;
    static float highestFPS = 0;

    std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();

    float currentDuration = std::chrono::duration_cast<std::chrono::seconds>(current - benchmarkData.beginTime).count();

    if (currentDuration > benchmarkData.duration)
    {
        i += 1;

        float           averageTime     = currentDuration / frameCount;
        float           averageFPS      = 1.0f / averageTime;
        BenchmarkResult benchmarkResult = {.name        = benchmarkData.renderingPipelines[i - 1].name,
                                           .totalTime   = currentDuration,
                                           .frameCount  = frameCount,
                                           .averageTime = averageTime * 1000,
                                           .averageFPS  = averageFPS,
                                           .lowestFPS   = lowestFPS,
                                           .highestFPS  = highestFPS};

        benchmarkData.results.push_back(benchmarkResult);

        if (i >= benchmarkData.renderingPipelines.size())
        {
            window.SetVSync(true);
            benchmarkData.isRunning = false;
            i                       = 0;
        }
        else
        {
            renderPasses = benchmarkData.renderingPipelines[i].renderPasses;
            switch (i)
            {
            case 0:
                DeleteDSRenderData(benchmarkData.dsRenderData);
                *benchmarkData.dsRenderData = CreateDSRenderData(window.GetProperties().Width, window.GetProperties().Height);
                break;
            case 1:
                *benchmarkData.forwardRenderData =
                    CreateForwardRenderData(window.GetProperties().Width, window.GetProperties().Height, 256);
                break;
            }
        }

        benchmarkData.beginTime = current;
        frameCount              = 0;
        lowestFPS               = 0;
        highestFPS              = 0;
    }
    else
    {
        frameCount++;
    }

    return i;
}