/*
 * This file was generated on 2023-10-22 17:27:59.371170
 * using the command: `/Users/jatin/ChowDSP/Research/NDK-Framework/generate_ndk_cpp.py ts_config.json`
 */
#include "TS_NDK.h"

namespace
{
// START USER ENTRIES
constexpr auto R5 = 10.0e3;
constexpr auto R4 = 4.7e3;
constexpr auto RL = 1.0e6;
constexpr auto C2 = 1.0e-6;
constexpr auto C3 = 0.047e-6;
constexpr auto C4 = 51.0e-12;
constexpr auto Vt = 26.0e-3;
constexpr auto Is_DP1 = 4.352e-9;
constexpr auto n_DP1 = 1.906;
constexpr auto Vb = 4.5;
// END USER ENTRIES
} // namespace

void TS_NDK::reset_state()
{
    // reset state vectors
    for (size_t ch = 0; ch < MAX_NUM_CHANNELS; ++ch)
    {
        x_n[ch] = Eigen::Vector<T, num_states> { 1.6115299199259692E-13, 0.040608000000007714, -2.4576912723008024E-17 };
        v_n[ch] = Eigen::Vector<T, num_nl_ports> { -2.5090771459586759E-12 };
    }
}

void TS_NDK::reset (T fs)
{
    const auto Gr = Eigen::DiagonalMatrix<T, num_resistors> ((T) 1 / R5, (T) 1 / R4, (T) 1 / RL);
    const auto Gx = Eigen::DiagonalMatrix<T, num_states> ((T) 2 * fs * C2, (T) 2 * fs * C3, (T) 2 * fs * C4);
    const auto Z = Eigen::DiagonalMatrix<T, num_states> (1, 1, 1);

    // Set up component-defined matrices
    const Eigen::Matrix<T, num_resistors, num_nodes> Nr {
        { +0, +1, +0, +0, +0, -1 },
        { +0, +0, +0, +1, +0, +0 },
        { +0, +0, +0, +0, +1, +0 },
    };

    const Eigen::Matrix<T, num_pots, num_nodes> Nv {
        { +0, +0, +1, +0, -1, +0 },
    };

    const Eigen::Matrix<T, num_states, num_nodes> Nx {
        { -1, +1, +0, +0, +0, +0 },
        { +0, +0, +1, -1, +0, +0 },
        { +0, +0, +1, +0, -1, +0 },
    };

    const Eigen::Matrix<T, num_voltages + num_op_amps, num_nodes> Nui {
        { +1, +0, +0, +0, +0, +0 },
        { +0, +0, +0, +0, +0, +1 },
        { +0, -1, +1, +0, +0, +0 },
    };

    const Eigen::Matrix<T, num_voltages + num_op_amps, num_nodes> Nuo {
        { +1, +0, +0, +0, +0, +0 },
        { +0, +0, +0, +0, +0, +1 },
        { +0, +0, +0, +0, +1, +0 },
    };

    const Eigen::Matrix<T, num_nl_ports, num_nodes> Nn {
        { +0, +0, +1, +0, -1, +0 },
    };

    const Eigen::Matrix<T, num_outputs, num_nodes> No {
        { +0, +0, +0, +0, +1, +0 },
    };

    static constexpr auto S_dim = num_nodes + num_voltages + num_op_amps;
    Eigen::Matrix<T, num_states, S_dim> Nx_0 = Eigen::Matrix<T, num_states, S_dim>::Zero();
    Nx_0.block<num_states, num_nodes> (0, 0) = Nx;
    Eigen::Matrix<T, num_nl_ports, S_dim> Nn_0 = Eigen::Matrix<T, num_nl_ports, S_dim>::Zero();
    Nn_0.block<num_nl_ports, num_nodes> (0, 0) = Nn;
    Eigen::Matrix<T, num_outputs, S_dim> No_0 = Eigen::Matrix<T, num_outputs, S_dim>::Zero();
    No_0.block<num_outputs, num_nodes> (0, 0) = No;
    Eigen::Matrix<T, num_pots, S_dim> Nv_0 = Eigen::Matrix<T, num_pots, S_dim>::Zero();
    Nv_0.block<num_pots, num_nodes> (0, 0) = Nv;
    Eigen::Matrix<T, num_voltages, S_dim> Zero_I = Eigen::Matrix<T, num_voltages, S_dim>::Zero();
    Zero_I.block<2, 2> (0, 6).setIdentity();

    Eigen::Matrix<T, S_dim, S_dim> S0_mat = Eigen::Matrix<T, S_dim, S_dim>::Zero();
    S0_mat.block<num_nodes, num_nodes> (0, 0) = Nr.transpose() * Gr * Nr + Nx.transpose() * Gx * Nx;
    S0_mat.block<num_nodes, num_voltages + num_op_amps> (0, num_nodes) = Nuo.transpose();
    S0_mat.block<num_voltages + num_op_amps, num_nodes> (num_nodes, 0) = Nui;
    Eigen::Matrix<T, S_dim, S_dim> S0_inv = S0_mat.inverse();

    // Pre-compute NDK and intermediate matrices
    Q = Nv_0 * (S0_inv * Nv_0.transpose());
    Ux = Nx_0 * (S0_inv * Nv_0.transpose());
    Uo = No_0 * (S0_inv * Nv_0.transpose());
    Un = Nn_0 * (S0_inv * Nv_0.transpose());
    Uu = Zero_I * (S0_inv * Nv_0.transpose());
    A0_mat = (T) 2 * (Z * (Gx * (Nx_0 * (S0_inv * Nx_0.transpose())))) - Z.toDenseMatrix();
    B0_mat = (T) 2 * (Z * (Gx * (Nx_0 * (S0_inv * Zero_I.transpose()))));
    C0_mat = (T) 2 * (Z * (Gx * (Nx_0 * (S0_inv * Nn_0.transpose()))));
    D0_mat = No_0 * (S0_inv * Nx_0.transpose());
    E0_mat = No_0 * (S0_inv * Zero_I.transpose());
    F0_mat = No_0 * (S0_inv * Nn_0.transpose());
    G0_mat = Nn_0 * (S0_inv * Nx_0.transpose());
    H0_mat = Nn_0 * (S0_inv * Zero_I.transpose());
    K0_mat = Nn_0 * (S0_inv * Nn_0.transpose());
    two_Z_Gx = (T) 2 * (Z.toDenseMatrix() * Gx.toDenseMatrix());

    reset_state();
}

void TS_NDK::update_pots (const std::array<T, num_pots>& pot_values)
{
    Eigen::Matrix<T, num_pots, num_pots> Rv = Eigen::Matrix<T, num_pots, num_pots>::Zero();
    Rv (0, 0) = pot_values[0]; // R6_P1
    Eigen::Matrix<T, num_pots, num_pots> Rv_Q_inv = (Rv + Q).inverse();

    A_mat = A0_mat - (two_Z_Gx * (Ux * (Rv_Q_inv * Ux.transpose())));
    Eigen::Matrix<T, num_states, num_voltages> B_mat = B0_mat - (two_Z_Gx * (Ux * (Rv_Q_inv * Uu.transpose())));
    B_mat_var = B_mat.leftCols<num_voltages_variable>();
    B_u_fix = B_mat.rightCols<num_voltages_constant>() * Eigen::Vector<T, num_voltages_constant> { Vb };
    C_mat = C0_mat - (two_Z_Gx * (Ux * (Rv_Q_inv * Un.transpose())));
    D_mat = D0_mat - (Uo * (Rv_Q_inv * Ux.transpose()));
    Eigen::Matrix<T, num_outputs, num_voltages> E_mat = E0_mat - (Uo * (Rv_Q_inv * Uu.transpose()));
    E_mat_var = E_mat.leftCols<num_voltages_variable>();
    E_u_fix = E_mat.rightCols<num_voltages_constant>() * Eigen::Vector<T, num_voltages_constant> { Vb };
    F_mat = F0_mat - (Uo * (Rv_Q_inv * Un.transpose()));
    G_mat = G0_mat - (Un * (Rv_Q_inv * Ux.transpose()));
    Eigen::Matrix<T, num_nl_ports, num_voltages> H_mat = H0_mat - (Un * (Rv_Q_inv * Uu.transpose()));
    H_mat_var = H_mat.leftCols<num_voltages_variable>();
    H_u_fix = H_mat.rightCols<num_voltages_constant>() * Eigen::Vector<T, num_voltages_constant> { Vb };
    K_mat = K0_mat - (Un * (Rv_Q_inv * Un.transpose()));
}

void TS_NDK::process (std::span<T> channel_data, size_t ch) noexcept
{
    Eigen::Vector<T, num_voltages_variable> u_n_var;
    Eigen::Vector<T, num_nl_ports> p_n;
    Eigen::Matrix<T, num_nl_ports, num_nl_ports> Jac = Eigen::Matrix<T, num_nl_ports, num_nl_ports>::Zero();
    Eigen::Vector<T, num_nl_ports> i_n;
    Eigen::Vector<T, num_nl_ports> F_min;
    Eigen::Matrix<T, num_nl_ports, num_nl_ports> A_solve;
    const Eigen::Matrix<T, num_nl_ports, num_nl_ports> eye = Eigen::Matrix<T, num_nl_ports, num_nl_ports>::Identity();
    Eigen::Vector<T, num_nl_ports> delta_v;
    Eigen::Vector<T, num_outputs> y_n;

    for (size_t n = 0; n < channel_data.size(); ++n)
    {
        u_n_var (0) = (T) channel_data[n];
        p_n.noalias() = G_mat * x_n[ch] + H_mat_var * u_n_var + H_u_fix;

        const auto sinh_cosh = [] (T x)
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

        T sinh_v0;
        T cosh_v0;
        const auto calc_currents = [&]
        {
            std::tie (sinh_v0, cosh_v0) = sinh_cosh (v_n[ch](0) / (n_DP1 * Vt));
            i_n (0) = (T) 2 * -Is_DP1 * sinh_v0;
        };

        const auto calc_jacobian = [&]
        {
            Jac (0, 0) = ((T) 2 * -Is_DP1 / (n_DP1 * Vt)) * cosh_v0;
        };

        T delta;
        int nIters = 0;
        do
        {
            calc_currents();
            calc_jacobian();

            F_min.noalias() = p_n + K_mat * i_n - v_n[ch];
            A_solve.noalias() = K_mat * Jac - eye;
            delta_v.noalias() = A_solve.householderQr().solve (F_min);
            v_n[ch] -= delta_v;
            delta = delta_v.array().abs().sum();
        } while (delta > 1.0e-3 && ++nIters < 10);
        calc_currents();

        y_n.noalias() = D_mat * x_n[ch] + E_mat_var * u_n_var + E_u_fix + F_mat * i_n;
        channel_data[n] = (T) y_n (0);
        x_n[ch] = A_mat * x_n[ch] + B_mat_var * u_n_var + B_u_fix + C_mat * i_n;
    }
}
