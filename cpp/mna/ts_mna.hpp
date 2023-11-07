#pragma once

#include <cmath>
#include <span>
#include <vector>

template <typename T = float>
struct TS_MNA
{
    void prepare (double sample_rate)
    {
        fs = static_cast<T> (sample_rate);

        gC2 = (T) 2 * fs * C2;
        gC2_R5_recip = (T) 1 / ((T) 1 + gC2 * R5);
        gC3 = (T) 2 * fs * C3;
        gC3_recip = (T) 1 / gC3;
        gC3_R4_recip = (T) 1 / ((T) 1 + gC3 * R4);
        gC4 = (T) 2 * fs * C4;

        iC2eq = (T) 0.0;
        iC3eq = (T) 0.040607851;
        iC4eq = (T) 1.69891257E-9;
        Vd = (T) 0.000173500041;

        std::vector<T> pre_buffer_data {};
        pre_buffer_data.resize (1000, (T) 4.5);
        process (pre_buffer_data, pre_buffer_data);
    }

    void set_distortion (float dist_01)
    {
        RP_recip = 1.0f / (R6 + dist_01 * P1);
    }

    void process (std::span<const T> in_data, std::span<T> out_data) noexcept
    {
        for (size_t n = 0; n < in_data.size(); ++n)
        {
            const auto Vi = in_data[n]; // set input voltage
            const auto Vplus = (gC2 * R5 * Vi - R5 * iC2eq + Vb) * gC2_R5_recip; // compute Vplus from Vi
            const auto Vminus = Vplus; // Vminus = Vplus (op-amp rule)

            const auto iC3 = (gC3 * Vminus - iC3eq) * gC3_R4_recip; // compute current through C3 (same as current through R4)

            int nIters = 0;
            T delta;
            do
            {
                // compute sinh and cosh at the same time so it's faster...
                const auto sinh_cosh = [] (T x) noexcept
                {
                    // ref: https://en.wikipedia.org/wiki/Hyperbolic_functions#Definitions
                    // sinh = (e^(2x) - 1) / (2e^x), cosh = (e^(2x) + 1) / (2e^x)
                    // let B = e^x, then sinh = (B^2 - 1) / (2B), cosh = (B^2 + 1) / (2B)
                    // simplifying, we get: sinh = 0.5 (B - 1/B), cosh = 0.5 (B + 1/B)

                    auto B = std::exp (x);
                    auto Br = (T) 0.5 / B;
                    B *= (T) 0.5;

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
            } while (std::abs (delta) > (T) 1.0e-3 && ++nIters < 10);

            // update capacitor states
            const auto vC2 = Vi - Vplus;
            iC2eq = (T) 2 * gC2 * vC2 - iC2eq;
            const auto vC3 = (iC3 + iC3eq) * gC3_recip;
            iC3eq = (T) 2 * gC3 * vC3 - iC3eq;
            iC4eq = (T) 2 * gC4 * Vd - iC4eq;

            out_data[n] = Vminus + Vd; // read output voltage
        }
    }

    T fs = {};

    // input -> plus-terminal:
    // constants
    static constexpr auto C2 = (T) 1.0e-6;
    static constexpr auto R5 = (T) 10.0e3;
    static constexpr auto Vb = (T) 4.5;
    // sample-rate dependent values
    T gC2 = (T) 2 * fs * C2;
    T gC2_R5_recip = (T) 1 / ((T) 1 + gC2 * R5);
    // state
    T iC2eq = {};

    // minus-terminal -> ground
    // constants
    static constexpr auto C3 = (T) 0.047e-6;
    static constexpr auto R4 = (T) 4.7e3;
    // sample-rate dependent values
    T gC3 = (T) 2 * fs * C3;
    T gC3_recip = (T) 1 / gC3;
    T gC3_R4_recip = (T) 1 / ((T) 1 + gC3 * R4);
    // state
    T iC3eq = {};

    // feedback path
    // constants
    static constexpr auto R6 = (T) 51.0e3;
    static constexpr auto P1 = (T) 500.0e3;
    static constexpr auto C4 = (T) 51.0e-12;
    static constexpr auto Vt = (T) 26.0e-3;
    static constexpr auto twoIs = (T) 2.0f * 4.352e-9;
    static constexpr auto nabla = (T) 1.906;
    static constexpr auto Vt_recip = (T) 1 / (nabla * Vt);
    // parameter-dependent values
    T RP_recip = (T) 1 / (R6 + (T) 0.1 * P1);
    // sample-rate dependent values
    float gC4 = (T) 2 * fs * C4;
    // state
    T iC4eq = {};
    T Vd = {}; // not really state, but allows us to use the previous value for an initial guess in the NR solver
};
