#include <chrono>
#include <random>
#include <vector>

#include "../mna/ts_mna.hpp"
#include "../ndk/TS_NDK.h"
#include "../rnn/ts_rnn.hpp"
#include "../wdf/ts_wdf.hpp"

static constexpr double fs = 96000.0;
static constexpr size_t num_samples = static_cast<size_t> (fs) * 200;

const auto input_data = []
{
    std::vector<float> signal;
    signal.resize (num_samples, {});

    for (size_t n = 0; n < num_samples; ++n)
        signal[n] = 0.2f * std::sin (2.0f * (float) M_PI * 200.0f * (float) n / (float) fs) + 4.5f;

    return signal;
}();

template <typename ModelType, typename... Args>
void run_model_bench (std::string_view name, Args&&... args)
{
    std::cout << "Running benchmark for model: " << name << std::endl;

    ModelType ts_model { std::forward<Args...> (args)... };
    ts_model.prepare (fs);
    ts_model.set_distortion (0.1f);

    std::vector<float> output_data {};
    output_data.resize (num_samples, 0.0f);

    namespace chrono = std::chrono;
    const auto start = chrono::high_resolution_clock::now();
    ts_model.process (input_data, output_data);
    const auto duration = chrono::high_resolution_clock::now() - start;

    const auto audio_ms = static_cast<double> (num_samples) / fs * 1000.0;
    const auto duration_ms = static_cast<double> (chrono::duration_cast<chrono::milliseconds> (duration).count());
    std::cout << "Processed " << audio_ms << " ms of audio in " << duration_ms << " ms, "
              << audio_ms / duration_ms << "x real-time" << std::endl;
}

void run_model_bench (std::string_view name)
{
    std::cout << "Running benchmark for model: " << name << std::endl;

    TS_NDK ts_model {};
    ts_model.reset (fs);
    ts_model.update_pots ({ TS_NDK::R6 + TS_NDK::P1 * 0.1f });

    std::vector<double> output_data {};
    output_data.resize (num_samples, 0.0f);
    std::copy (input_data.begin(), input_data.end(), output_data.begin());

    namespace chrono = std::chrono;
    const auto start = chrono::high_resolution_clock::now();
    ts_model.process (output_data, 0);
    const auto duration = chrono::high_resolution_clock::now() - start;

//    std::cout << "Final Sample: " << output_data.back() << std::endl;
    const auto audio_ms = static_cast<double> (num_samples) / fs * 1000.0;
    const auto duration_ms = static_cast<double> (chrono::duration_cast<chrono::milliseconds> (duration).count());
    std::cout << "Processed " << audio_ms << " ms of audio in " << duration_ms << " ms, "
              << audio_ms / duration_ms << "x real-time" << std::endl;
}

int main()
{
    run_model_bench<TS_WDF<WDF_Mode::WDF, float>> ("WDF");
    run_model_bench<TS_MNA<float>> ("MNA");
    run_model_bench<TS_RNN<12>> ("RNN-12", "model_best_12.json");
    run_model_bench<TS_RNN<24>> ("RNN-24", "model_best_24.json");
    run_model_bench<TS_WDF<WDF_Mode::DWDF, float>> ("DWDF");
    run_model_bench ("NDK");
}

// Results:
// Running benchmark for model: WDF
// Processed 200000 ms of audio in 1121 ms, 178.412x real-time
// Running benchmark for model: MNA
// Processed 200000 ms of audio in 977 ms, 204.708x real-time
// Running benchmark for model: RNN-12
// Processed 200000 ms of audio in 2073 ms, 96.4785x real-time
// Running benchmark for model: RNN-24
// Processed 200000 ms of audio in 4533 ms, 44.1209x real-time
// Running benchmark for model: DWDF
// Processed 200000 ms of audio in 1581 ms, 126.502x real-time4
// Running benchmark for model: NDK
// Processed 200000 ms of audio in 1819 ms, 109.951x real-time
