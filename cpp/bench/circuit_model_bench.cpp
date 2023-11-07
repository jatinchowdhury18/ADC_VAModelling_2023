#include <array>
#include <random>

#include <benchmark/benchmark.h>

#include "../mna/ts_mna.hpp"
#include "../ndk/TS_NDK.h"
#include "../rnn/ts_rnn.hpp"
#include "../wdf/ts_wdf.hpp"

static constexpr double fs = 96000.0;
static constexpr size_t num_samples = static_cast<size_t> (fs) * 10;

const auto input_data = []
{
    std::vector<float> signal;
    signal.resize (num_samples, {});

    std::random_device dev;
    std::mt19937 rng { dev() };
    std::uniform_real_distribution<float> dist { 4.3f, 4.7f };

    for (auto& sample : signal)
    {
        sample = dist (rng);
    }

    return signal;
}();

template <typename ModelType>
void run_model_bench (benchmark::State& state, ModelType& ts_model)
{
    ts_model.prepare (fs);
    ts_model.set_distortion (0.1f);

    std::array<float, num_samples> output_data {};
    for (auto _ : state)
    {
        ts_model.process (input_data, output_data);
        benchmark::DoNotOptimize (output_data);
    }
}

static void ts_wdf (benchmark::State& state)
{
    TS_WDF<float> ts_model;
    run_model_bench (state, ts_model);
}
BENCHMARK (ts_wdf)->MinTime (5.0);

static void ts_mna (benchmark::State& state)
{
    TS_MNA<float> ts_model;
    run_model_bench (state, ts_model);
}
BENCHMARK (ts_mna)->MinTime (5.0);

static void ts_ndk (benchmark::State& state)
{
    TS_NDK ts_model;
    ts_model.reset (fs);
    ts_model.update_pots ({ TS_NDK::P1 * 0.1f + TS_NDK::R6 });

    std::array<double, num_samples> output_data {};
    for (auto _ : state)
    {
        std::copy (input_data.begin(), input_data.end(), output_data.begin());
        ts_model.process (output_data, 0);
        benchmark::DoNotOptimize (output_data);
    }
}
BENCHMARK (ts_ndk)->MinTime (5.0);

static void ts_rnn (benchmark::State& state)
{
    TS_RNN<24> ts_model { "model_best_24.json" };
    run_model_bench (state, ts_model);
}
BENCHMARK (ts_rnn)->MinTime (5.0);

BENCHMARK_MAIN();
