#pragma once

#include <span>
#include <vector>

struct TS_MNA
{
    void prepare (double sample_rate)
    {
        fs = static_cast<float> (sample_rate);

        gC2 = 2.0f * fs * C2;
        gC2_R5_recip = 1.0f / (1.0f + gC2 * R5);
        gC3 = 2.0f * fs * C3;
        gC3_recip = 1.0f / gC3;
        gC3_R4_recip = 1.0f / (1.0f + gC3 * R4);
        gC4 = 2.0f * fs * C4;

        iC2eq = 0.0f;
        iC3eq = 0.040607851f;
        iC4eq = 1.69891257E-9f;
        Vd = 0.000173500041f;

        std::vector<float> pre_buffer_data {};
        pre_buffer_data.resize (1000, 4.5f);
        process (pre_buffer_data, pre_buffer_data);
    }

    void set_distortion (float dist_01)
    {
        RP_recip = 1.0f / (R6 + dist_01 * P1);
    }

    void process (std::span<const float> in_data, std::span<float> out_data) noexcept
    {
        for (size_t n = 0; n < in_data.size(); ++n)
        {
            const auto Vi = in_data[n]; // set input voltage
            const auto Vplus = (gC2 * R5 * Vi - R5 * iC2eq + Vb) * gC2_R5_recip; // compute Vplus from Vi
            const auto Vminus = Vplus; // Vminus = Vplus (op-amp rule)

            const auto iC3 = (gC3 * Vminus - iC3eq) * gC3_R4_recip; // compute current through C3 (same as current through R4)

            int nIters = 0;
            float delta;
            do
            {
                // compute sinh and cosh at the same time so it's faster...
                const auto sinh_cosh = [] (float x) noexcept
                {
                    // ref: https://en.wikipedia.org/wiki/Hyperbolic_functions#Definitions
                    // sinh = (e^(2x) - 1) / (2e^x), cosh = (e^(2x) + 1) / (2e^x)
                    // let B = e^x, then sinh = (B^2 - 1) / (2B), cosh = (B^2 + 1) / (2B)
                    // simplifying, we get: sinh = 0.5 (B - 1/B), cosh = 0.5 (B + 1/B)

                    auto B = std::exp (x);
                    auto Br = 0.5f / B;
                    B *= 0.5f;

                    auto sinh = B - Br;
                    auto cosh = B + Br;

                    return std::make_pair (sinh, cosh);
                };

                // iC3 = iRP + iC4 + iD (Kichoff current law)
                // NR-solver with F(Vd) = iRP + iC4 - iC3 + iD
                // allows us to solve for Vd

                const auto [sinh_Vd, cosh_Vd] = sinh_cosh (Vd * Vt_recip);
                const auto F = Vd * RP_recip + gC4 * Vd - iC4eq - iC3 + twoIs * sinh_Vd;
                const auto F_deriv = RP_recip + gC4 + (twoIs * Vt_recip) * cosh_Vd;
                delta = F / F_deriv;
                Vd -= delta;
            } while (std::abs (delta) > 1.0e-5f && ++nIters < 10);

            // update capacitor states
            const auto vC2 = Vi - Vplus;
            iC2eq = 2.0f * gC2 * vC2 - iC2eq;
            const auto vC3 = (iC3 + iC3eq) * gC3_recip;
            iC3eq = 2.0f * gC3 * vC3 - iC3eq;
            iC4eq = 2.0f * gC4 * Vd - iC4eq;

            out_data[n] = Vminus + Vd; // read output voltage
        }
    }

    float fs = 0.0f;

    // input -> plus-terminal:
    // constants
    static constexpr auto C2 = 1.0e-6f;
    static constexpr auto R5 = 10.0e3f;
    static constexpr auto Vb = 4.5f;
    // sample-rate dependent values
    float gC2 = 2.0f * fs * C2;
    float gC2_R5_recip = 1.0f / (1.0f + gC2 * R5);
    // state
    float iC2eq = 0.0f;

    // minus-terminal -> ground
    // constants
    static constexpr auto C3 = 0.047e-6f;
    static constexpr auto R4 = 4.7e3f;
    // sample-rate dependent values
    float gC3 = 2.0f * fs * C3;
    float gC3_recip = 1.0f / gC3;
    float gC3_R4_recip = 1.0f / (1.0f + gC3 * R4);
    // state
    float iC3eq = 0.0f;

    // feedback path
    // constants
    static constexpr auto R6 = 51.0e3f;
    static constexpr auto P1 = 500.0e3f;
    static constexpr auto C4 = 51.0e-12f;
    static constexpr auto Vt = 26.0e-3f;
    static constexpr auto twoIs = 2.0f * 4.352e-9f;
    static constexpr auto nabla = 1.906f;
    static constexpr auto Vt_recip = 1.0f / (nabla * Vt);
    // parameter-dependent values
    float RP_recip = 1.0f / (R6 + 0.1f * P1);
    // sample-rate dependent values
    float gC4 = 2.0f * fs * C4;
    // state
    float iC4eq = 0.0f;
    float Vd = 0.0f; // not really state, but allows us to use the previous value for an initial guess in the NR solver
};
