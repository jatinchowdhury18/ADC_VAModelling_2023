#include <cmath>
#include <iostream>

#include <plt/matplotlibcpp.h>
namespace plt = matplotlibcpp;

#include "wdf/ds1_wdf.hpp"

static constexpr auto sample_rate = 96000.0f;
static constexpr auto test_freq = 100.0f;

int main (int argc, char* argv[])
{
    std::vector<float> data {};
    data.resize (static_cast<size_t> (sample_rate * 0.05f));
    for (size_t n = 0; n < data.size(); ++n)
        data[n] = 1.5f * std::sin (2.0f * (float) M_PI * (float) n * test_freq / sample_rate) + 0.0f;
    plt::named_plot<float> ("Input", data);

    DS1_WDF ds1_wdf {};
    for (auto distortion_val : { 0.0f, 0.3333f, 0.6667f, 1.0f })
    {
        ds1_wdf.set_distortion_control (distortion_val);
        ds1_wdf.prepare (sample_rate);
        std::vector<float> wdf_out;
        wdf_out.resize (data.size());
        ds1_wdf.process (data, wdf_out);
        plt::named_plot<float> ("Distortion: " + std::to_string (distortion_val), wdf_out);
    }

    plt::legend ();
    plt::show();

    return 0;
}
