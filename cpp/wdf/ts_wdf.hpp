#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>
#include <span>

namespace wdft = chowdsp::wdft;

struct TS_WDF
{
    void prepare (double sample_rate)
    {
        R5.setVoltage (4.5f);
        Vin_C2.prepare ((float) sample_rate);
        R4_ser_C3.prepare ((float) sample_rate);
        R6_P1_par_C4.prepare ((float) sample_rate);

        std::vector<float> pre_buffer_data {};
        pre_buffer_data.resize (1000, 4.5f);
        process (pre_buffer_data, pre_buffer_data);
    }

    void set_distortion (float dist_01)
    {
        R6_P1_par_C4.setResistanceValue (Pot1 * dist_01 + R6);
    }

    void process (std::span<const float> in_data, std::span<float> out_data) noexcept
    {
        for (size_t n = 0; n < in_data.size(); ++n)
        {
            Vin_C2.setVoltage (in_data[n]);

            dp.incident (P3.reflected());
            P3.incident (dp.reflected());

            out_data[n] = wdft::voltage<float> (RL);
        }
    }

    // Port B
    wdft::CapacitiveVoltageSourceT<float> Vin_C2 { 1.0e-6f };
    wdft::ResistiveVoltageSourceT<float> R5 { 10.0e3f };
    wdft::WDFParallelT<float, decltype (Vin_C2), decltype (R5)> P1 { Vin_C2, R5 };

    // Port C
    wdft::ResistorCapacitorSeriesT<float> R4_ser_C3 { 4.7e3f, 0.047e-6f };

    // Port D
    wdft::ResistorT<float> RL { 1.0e6f };

    struct ImpedanceCalc
    {
        template <typename RType>
        static float calcImpedance (RType& R)
        {
            constexpr float Ag = 1000.0f; // op-amp gain
            constexpr float Ri = 5.0e6f; // op-amp input impedance
            constexpr float Ro = 2.0e-1f; // op-amp output impedance

            const auto [Rb, Rc, Rd] = R.getPortImpedances();

            // This scattering matrix was derived using the R-Solver python script (https://github.com/jatinchowdhury18/R-Solver),
            // invoked with command: r_solver.py --adapt 0 --out scratch/tube_screamer_scatt.txt scratch/tube_screamer.txt
            R.setSMatrixData ({ { 0, (Ag * Rd * Ri - Rc * Rd + Rc * Ro) / ((Rb + Rc) * Rd + Rd * Ri - (Rb + Rc + Ri) * Ro), -((Ag + 1) * Rd * Ri + Rb * Rd - (Rb + Ri) * Ro) / ((Rb + Rc) * Rd + Rd * Ri - (Rb + Rc + Ri) * Ro), -Ro / (Rd - Ro) },
                                { -(Rb * Rc * Rd - Rb * Rc * Ro) / ((Ag + 1) * Rc * Rd * Ri + Rb * Rc * Rd - (Rb * Rc + (Rb + Rc) * Rd + (Rc + Rd) * Ri) * Ro), ((Ag + 1) * Rc * Rc * Rd * Ri + (Ag + 1) * Rc * Rd * Ri * Ri - Rb * Rb * Rc * Rd + (Rb * Rb * Rc - (Rc + Rd) * Ri * Ri + (Rb * Rb - Rc * Rc) * Rd - (Rc * Rc + 2 * Rc * Rd) * Ri) * Ro) / ((Ag + 1) * Rc * Rd * Ri * Ri + ((Ag + 2) * Rb * Rc + (Ag + 1) * Rc * Rc) * Rd * Ri + (Rb * Rb * Rc + Rb * Rc * Rc) * Rd - (Rb * Rb * Rc + Rb * Rc * Rc + (Rc + Rd) * Ri * Ri + (Rb * Rb + 2 * Rb * Rc + Rc * Rc) * Rd + (2 * Rb * Rc + Rc * Rc + 2 * (Rb + Rc) * Rd) * Ri) * Ro), ((Ag + 1) * Rb * Rc * Rd * Ri + Rb * Rb * Rc * Rd - (Rb * Rb * Rc + 2 * (Rb * Rb + Rb * Rc) * Rd + (Rb * Rc + 2 * Rb * Rd) * Ri) * Ro) / ((Ag + 1) * Rc * Rd * Ri * Ri + ((Ag + 2) * Rb * Rc + (Ag + 1) * Rc * Rc) * Rd * Ri + (Rb * Rb * Rc + Rb * Rc * Rc) * Rd - (Rb * Rb * Rc + Rb * Rc * Rc + (Rc + Rd) * Ri * Ri + (Rb * Rb + 2 * Rb * Rc + Rc * Rc) * Rd + (2 * Rb * Rc + Rc * Rc + 2 * (Rb + Rc) * Rd) * Ri) * Ro), -Rb * Rc * Ro / ((Ag + 1) * Rc * Rd * Ri + Rb * Rc * Rd - (Rb * Rc + (Rb + Rc) * Rd + (Rc + Rd) * Ri) * Ro) },
                                { -(Rb * Rc * Rd + Rc * Rd * Ri - (Rb * Rc + Rc * Ri) * Ro) / ((Ag + 1) * Rc * Rd * Ri + Rb * Rc * Rd - (Rb * Rc + (Rb + Rc) * Rd + (Rc + Rd) * Ri) * Ro), (Ag * Rc * Rd * Ri * Ri + Rb * Rc * Rc * Rd + (Ag * Rb * Rc + (2 * Ag + 1) * Rc * Rc) * Rd * Ri - (Rb * Rc * Rc + 2 * (Rb * Rc + Rc * Rc) * Rd + (Rc * Rc + 2 * Rc * Rd) * Ri) * Ro) / ((Ag + 1) * Rc * Rd * Ri * Ri + ((Ag + 2) * Rb * Rc + (Ag + 1) * Rc * Rc) * Rd * Ri + (Rb * Rb * Rc + Rb * Rc * Rc) * Rd - (Rb * Rb * Rc + Rb * Rc * Rc + (Rc + Rd) * Ri * Ri + (Rb * Rb + 2 * Rb * Rc + Rc * Rc) * Rd + (2 * Rb * Rc + Rc * Rc + 2 * (Rb + Rc) * Rd) * Ri) * Ro), -((Ag + 1) * Rc * Rc * Rd * Ri + Rb * Rc * Rc * Rd - (Rb * Rc * Rc - Rd * Ri * Ri - (Rb * Rb - Rc * Rc) * Rd + (Rc * Rc - 2 * Rb * Rd) * Ri) * Ro) / ((Ag + 1) * Rc * Rd * Ri * Ri + ((Ag + 2) * Rb * Rc + (Ag + 1) * Rc * Rc) * Rd * Ri + (Rb * Rb * Rc + Rb * Rc * Rc) * Rd - (Rb * Rb * Rc + Rb * Rc * Rc + (Rc + Rd) * Ri * Ri + (Rb * Rb + 2 * Rb * Rc + Rc * Rc) * Rd + (2 * Rb * Rc + Rc * Rc + 2 * (Rb + Rc) * Rd) * Ri) * Ro), -(Rb * Rc + Rc * Ri) * Ro / ((Ag + 1) * Rc * Rd * Ri + Rb * Rc * Rd - (Rb * Rc + (Rb + Rc) * Rd + (Rc + Rd) * Ri) * Ro) },
                                { (Ag * Rc * Rd * Ri - ((Rb + Rc) * Rd + Rd * Ri) * Ro) / ((Ag + 1) * Rc * Rd * Ri + Rb * Rc * Rd - (Rb * Rc + (Rb + Rc) * Rd + (Rc + Rd) * Ri) * Ro), ((Ag * Ag + 2 * Ag) * Rc * Rd * Rd * Ri * Ri + (2 * Ag * Rb * Rc + Ag * Rc * Rc) * Rd * Rd * Ri + (Rc * Rd * Ri + (Rb * Rc + Rc * Rc) * Rd) * Ro * Ro - ((Rb * Rc + Rc * Rc) * Rd * Rd + (2 * Ag * Rc * Rd + Ag * Rd * Rd) * Ri * Ri + ((Ag * Rb + (Ag + 1) * Rc) * Rd * Rd + (2 * Ag * Rb * Rc + Ag * Rc * Rc) * Rd) * Ri) * Ro) / ((Ag + 1) * Rc * Rd * Rd * Ri * Ri + ((Ag + 2) * Rb * Rc + (Ag + 1) * Rc * Rc) * Rd * Rd * Ri + (Rb * Rb * Rc + Rb * Rc * Rc) * Rd * Rd + (Rb * Rb * Rc + Rb * Rc * Rc + (Rc + Rd) * Ri * Ri + (Rb * Rb + 2 * Rb * Rc + Rc * Rc) * Rd + (2 * Rb * Rc + Rc * Rc + 2 * (Rb + Rc) * Rd) * Ri) * Ro * Ro - ((Rb * Rb + 2 * Rb * Rc + Rc * Rc) * Rd * Rd + ((Ag + 2) * Rc * Rd + Rd * Rd) * Ri * Ri + 2 * (Rb * Rb * Rc + Rb * Rc * Rc) * Rd + (2 * (Rb + Rc) * Rd * Rd + ((Ag + 4) * Rb * Rc + (Ag + 2) * Rc * Rc) * Rd) * Ri) * Ro), -(Ag * Rb * Rc * Rd * Rd * Ri + (Ag * Ag + Ag) * Rc * Rd * Rd * Ri * Ri - ((2 * Rb + Rc) * Rd * Ri + Rd * Ri * Ri + (Rb * Rb + Rb * Rc) * Rd) * Ro * Ro + ((Rb * Rb + Rb * Rc) * Rd * Rd - (Ag * Rc * Rd + (Ag - 1) * Rd * Rd) * Ri * Ri - (Ag * Rb * Rc * Rd + ((Ag - 2) * Rb + (Ag - 1) * Rc) * Rd * Rd) * Ri) * Ro) / ((Ag + 1) * Rc * Rd * Rd * Ri * Ri + ((Ag + 2) * Rb * Rc + (Ag + 1) * Rc * Rc) * Rd * Rd * Ri + (Rb * Rb * Rc + Rb * Rc * Rc) * Rd * Rd + (Rb * Rb * Rc + Rb * Rc * Rc + (Rc + Rd) * Ri * Ri + (Rb * Rb + 2 * Rb * Rc + Rc * Rc) * Rd + (2 * Rb * Rc + Rc * Rc + 2 * (Rb + Rc) * Rd) * Ri) * Ro * Ro - ((Rb * Rb + 2 * Rb * Rc + Rc * Rc) * Rd * Rd + ((Ag + 2) * Rc * Rd + Rd * Rd) * Ri * Ri + 2 * (Rb * Rb * Rc + Rb * Rc * Rc) * Rd + (2 * (Rb + Rc) * Rd * Rd + ((Ag + 4) * Rb * Rc + (Ag + 2) * Rc * Rc) * Rd) * Ri) * Ro), -((Ag + 1) * Rc * Rd * Rd * Ri + Rb * Rc * Rd * Rd - (Rb * Rc + Rc * Ri) * Ro * Ro - ((Rb + Rc) * Rd * Rd + Rd * Rd * Ri) * Ro) / ((Ag + 1) * Rc * Rd * Rd * Ri + Rb * Rc * Rd * Rd + (Rb * Rc + (Rb + Rc) * Rd + (Rc + Rd) * Ri) * Ro * Ro - (2 * Rb * Rc * Rd + (Rb + Rc) * Rd * Rd + ((Ag + 2) * Rc * Rd + Rd * Rd) * Ri) * Ro) } });

            const auto Ra = ((Ag + 1) * Rc * Rd * Ri + Rb * Rc * Rd - (Rb * Rc + (Rb + Rc) * Rd + (Rc + Rd) * Ri) * Ro) / ((Rb + Rc) * Rd + Rd * Ri - (Rb + Rc + Ri) * Ro);
            return Ra;
        }
    };

    wdft::RtypeAdaptor<float, 0, ImpedanceCalc, decltype (P1), decltype (R4_ser_C3), decltype (RL)> R { P1, R4_ser_C3, RL };

    // Port A
    static constexpr auto R6 = 51.0e3f;
    static constexpr auto Pot1 = 500.0e3f;
    wdft::ResistorCapacitorParallelT<float> R6_P1_par_C4 { R6, 51.0e-12f };
    wdft::WDFParallelT<float, decltype (R6_P1_par_C4), decltype (R)> P3 { R6_P1_par_C4, R };

    static constexpr float Vt = 26.0e-3f;
    wdft::DiodePairT<float, decltype (P3)> dp { P3, 4.352e-9f, Vt, 1.906f }; // 1N4148
};
