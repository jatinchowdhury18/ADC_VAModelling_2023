#include <cmath>
#include <iostream>

#include <plt/matplotlibcpp.h>
namespace plt = matplotlibcpp;

#include "ndk/TS_NDK.h"
#include "wdf/ts_wdf.hpp"
#include "rnn/ts_rnn.hpp"
#include "mna/ts_mna.hpp"

static constexpr auto sample_rate = 96000.0f;
static constexpr auto test_freq = 100.0f;

int main (int argc, char* argv[])
{
    if (argc == 0 || std::string { argv[1] } == "--help")
    {
        std::cout << "Options: --wdf, --rnn, --mna, or --ndk" << std::endl;
        return 1;
    }

    plt::figure_size (1000, 750);
    plt::xlabel ("Time [samples]");
    plt::ylabel ("Signal [V]");

    const auto data = []
    {
        std::vector<float> data {};
        data.resize (static_cast<size_t> (sample_rate * 0.05f));
        for (size_t n = 0; n < data.size(); ++n)
            data[n] = 0.2f * std::sin (2.0f * (float) M_PI * (float) n * test_freq / sample_rate) + 4.5f;
        return data;
    }();
    plt::named_plot<float> ("Input", data);

    if (std::string { argv[1] } == "--wdf")
    {
        TS_WDF ts_wdf {};
        for (auto distortion_val : { 0.0f, 0.1f, 1.0f })
        {
            ts_wdf.set_distortion (distortion_val);
            ts_wdf.prepare (sample_rate);
            std::vector<float> wdf_out;
            wdf_out.resize (data.size());
            ts_wdf.process (data, wdf_out);

            const auto plot_name = "P1 = " + std::to_string (static_cast<int> (TS_WDF<float>::Pot1 * distortion_val) / 1000) + " KOhms";
            plt::named_plot<float> (plot_name, wdf_out);
        }
    }
    else if (std::string { argv[1] } == "--ndk")
    {
        TS_NDK ts_ndk {};
        for (auto distortion_val : { 0.0f, 0.1f, 1.0f })
        {
            ts_ndk.reset (sample_rate);
            ts_ndk.update_pots ({ TS_NDK::P1 * distortion_val + TS_NDK::R6 });
            std::vector<double> ndk_out { data.begin(), data.end() };
            std::transform (ndk_out.begin(), ndk_out.end(), ndk_out.begin(), [] (double x) { return (x - 4.5) * 0.25 + 4.5; });
            ts_ndk.process (ndk_out, 0);
            std::transform (ndk_out.begin(), ndk_out.end(), ndk_out.begin(), [] (double x) { return (x - 4.5) * 1.4 + 4.5; });
            plt::named_plot<double> ("Distortion: " + std::to_string (distortion_val), ndk_out);
        }
    }
    else if (std::string { argv[1] } == "--rnn")
    {
        TS_RNN<24> ts_rnn { "model_best_24.json" };
        for (auto distortion_val : { 0.0f, 0.1f, 1.0f })
        {
            ts_rnn.set_distortion (distortion_val);
            ts_rnn.prepare (sample_rate);
            std::vector<float> rnn_out;
            rnn_out.resize (data.size());
            ts_rnn.process (data, rnn_out);

            const auto plot_name = "P1 = " + std::to_string (static_cast<int> (TS_WDF<float>::Pot1 * distortion_val) / 1000) + " KOhms";
            plt::named_plot<float> (plot_name, rnn_out);
        }
    }
    else if (std::string { argv[1] } == "--mna")
    {
        TS_MNA ts_mna;
        for (auto distortion_val : { 0.0f, 0.1f, 1.0f })
        {
            ts_mna.set_distortion (distortion_val);
            ts_mna.prepare (sample_rate);
            std::vector<float> mna_out;
            mna_out.resize (data.size());
            ts_mna.process (data, mna_out);

            const auto plot_name = "P1 = " + std::to_string (static_cast<int> (TS_WDF<float>::Pot1 * distortion_val) / 1000) + " KOhms";
            plt::named_plot<float> (plot_name, mna_out);
        }
    }

//    plt::ylim (3.5, 5.5);
    plt::ylim (3.75, 5.25);
    plt::xlim (1000, 4500);
    plt::legend();
    plt::grid (true);
    plt::show();

    return 0;
}
