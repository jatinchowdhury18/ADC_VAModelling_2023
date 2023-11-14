#pragma once

#include <cmath>
#include <span>

struct TS_Sym
{
    float x[10] {};

    void prepare (double sample_rate)
    {
        std::fill (std::begin (x), std::end (x), 0.0f);
    }

    void process (std::span<const float> in_data, std::span<float> out_data) noexcept
    {
        const auto cube = [] (float x)
        {
            return x * x * x;
        };

        const auto sign = [] (float x)
        {
            return x >= 0.0f ? 1.0f : -1.0f;
        };

        const auto atanh_clip = [] (float x)
        {
            const auto xp1 = x + 1.0f;
            const auto xp1_mod = xp1 - 2.0f * float (int (xp1 * 0.5f));
            return std::atanh (xp1_mod - 1.0f);
        };

        for (size_t n = 0; n < in_data.size(); ++n)
        {
            x[0] = 5.0f * (in_data[n] - 4.5f);
            out_data[n] = std::tanh(x[3] - std::erf(cube(std::sin((atanh_clip(x[8]) * 0.60346466f) - std::ceil(sign(atanh_clip(cube(std::erf(x[2] - x[9])))))))));
            out_data[n] = out_data[n] / 1.55f + 4.5f;

            // update states
            x[9] = x[8];
            x[8] = x[7];
            x[7] = x[6];
            x[6] = x[5];
            x[5] = x[4];
            x[4] = x[3];
            x[3] = x[2];
            x[2] = x[0];
        }
    }
};
