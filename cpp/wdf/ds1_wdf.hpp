#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>
#include <span>

struct DS1_WDF
{
    void prepare (float sample_rate)
    {
        VR1m_C7.prepare (sample_rate);
        R13_VR1p_C8.prepare (sample_rate);
        R14_C9.prepare (sample_rate);
        C10.prepare (sample_rate);
    }

    void set_distortion_control (float value01)
    {
        VR1m_C7.setResistanceValue (100.0e3f * value01);
        R13_VR1p_C8.setResistanceValue (100.0e3f * (1.0f - value01) + + 4.7e3f);
    }

    void process (std::span<const float> in_data, std::span<float> out_data)
    {
        for (size_t n = 0; n < in_data.size(); ++n)
        {
            v_in.setVoltage (in_data[n]);

            dp.incident (parallel_e.reflected());
            parallel_e.incident (dp.reflected());

            out_data[n] = chowdsp::wdft::voltage<float> (C10);
        }
    }

    // Port A
    chowdsp::wdft::ResistiveVoltageSourceT<float> v_in {};

    // Port B:
    chowdsp::wdft::ResistorT<float> R11 { 100.0e3f };

    // Port C:
    chowdsp::wdft::ResistorCapacitorParallelT<float> VR1m_C7 { 50.0e3f, 100.0e-9f };

    // Port D:
    chowdsp::wdft::ResistorCapacitorSeriesT<float> R13_VR1p_C8 { 50.0e3f + 4.7e3f, 0.47e-6f };

    // R-Type Adaptor
    struct ImpedanceCalculator
    {
        template <typename RType>
        static float calcImpedance (RType& R)
        {
            constexpr float Ag = 100.0f; // op-amp gain
            constexpr float Ri = 1.0e9f; // op-amp input impedance
            constexpr float Ro = 1.0e-1f; // op-amp output impedance

            const auto [Ra, Rb, Rc, Rd] = R.getPortImpedances();

            // R-Type scattering matrix derived from r-solver.nb
            // "ds1_netlist.txt", adapting port #5
            const auto Re = -(((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro) / (Ra * Rc + Rb * Rc + Ra * Rd + Rb * Rd + Rc * Rd + Rc * Ri + Rd * Ri + Ag * Rd * Ri - (Ra + Rb + Rd + Ri) * Ro));
            R.setSMatrixData ({ { (Ra * Re * (Rc + Rd - Ro) + (Rc * Rd + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro), (Ra * (Rc + Rd) * Re - Ra * (Rc + Rd + Re) * Ro) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro), (Ra * Rd * (-Re + Ro)) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro), (-(Ra * Rc * Re) + Ra * (Rc + Re) * Ro) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro), (Ra * Rd) / (Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) },
                                { (Rb * (Rc + Rd) * Re - Rb * (Rc + Rd + Re) * Ro) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro), (Rb * Re * (Rc + Rd - Ro) + (Rc * Rd + Ra * (Rc + Rd) + (Rc + Rd) * Ri) * Ro) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro), (Rb * Rd * (-Re + Ro)) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro), (-(Rb * Rc * Re) + Rb * (Rc + Re) * Ro) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro), (Rb * Rd) / (Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) },
                                { (Rc * (Ag * Re * Ri + Rd * (-Re + Ro))) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro), (Rc * (Ag * Re * Ri + Rd * (-Re + Ro))) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro), (Rc * Re * (Ra + Rb + Rd + Ri) + Rd * (Ra + Rb + Ri) * Ro) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro), (-(Rc * Re * (Ra + Rb + Ri + Ag * Ri)) + Rc * (Ra + Rb + Ri) * Ro) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro), -((Rc * (Ra + Rb + Rd + Ri)) / (Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri)) },
                                { (-(Rd * Re * (Rc + Ag * Ri)) + Rd * (Rc + Re) * Ro) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro), (-(Rd * Re * (Rc + Ag * Ri)) + Rd * (Rc + Re) * Ro) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro), -((Rd * (Ra + Rb + Ri) * (Re - Ro)) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro)), -((Rc * Re * (Ra + Rb + Ri) + (Rc * Rd + Ra * (Rd - Re) + Rb * (Rd - Re) + (Rd - Re) * Ri) * Ro) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro)), (Rd * (Ra + Rb + Ri)) / (Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) },
                                { (-(Ag * (Rc + Rd) * Re * Ri) + Rd * Re * Ro) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro), (-(Ag * (Rc + Rd) * Re * Ri) + Rd * Re * Ro) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro), -((Re + Ro) / Ro), (Re * (Ag * Rc * Ri + (Ra + Rb + Ri) * Ro)) / ((Rc * Rd + Ra * (Rc + Rd) + Rb * (Rc + Rd) + (Rc + Rd) * Ri) * Ro), 0 } });
            return Re;
        }
    };
    chowdsp::wdft::RtypeAdaptor<float,
                                4,
                                ImpedanceCalculator,
                                decltype (v_in),
                                decltype (R11),
                                decltype (VR1m_C7),
                                decltype (R13_VR1p_C8)>
        r_adaptor { v_in, R11, VR1m_C7, R13_VR1p_C8 };

    // Port E:
    chowdsp::wdft::ResistorCapacitorSeriesT<float> R14_C9 { 2.2e3f, 0.47e-6f };
    chowdsp::wdft::WDFSeriesT<float, decltype (R14_C9), decltype (r_adaptor)> series_e { R14_C9, r_adaptor };

    chowdsp::wdft::CapacitorT<float> C10 { 0.01e-6f };
    chowdsp::wdft::WDFParallelT<float, decltype (C10), decltype (series_e)> parallel_e { C10, series_e };

    chowdsp::wdft::DiodePairT<float, decltype (parallel_e)> dp { parallel_e, 4.352E-9f, 25.85e-3f, 1.906f };
};
