/*
 * This file was generated on 2023-10-22 17:27:59.370973
 * using the command: `/Users/jatin/ChowDSP/Research/NDK-Framework/generate_ndk_cpp.py ts_config.json`
 */
#pragma once

#include <array>
#include <cmath>
#include <span>

// START USER INCLUDES
#include <Eigen/Dense>
// END USER INCLUDES

struct TS_NDK
{
    // START USER ENTRIES
    static constexpr size_t MAX_NUM_CHANNELS = 1;
    static constexpr auto R6 = 51.0e3;
    static constexpr auto P1 = 500.0e3;
    // END USER ENTRIES

    using T = double;
    static constexpr int num_nodes = 6;
    static constexpr int num_resistors = 3;
    static constexpr int num_pots = 1;
    static constexpr int num_states = 3;
    static constexpr int num_nl_ports = 1;
    static constexpr int num_op_amps = 1;
    static constexpr int num_voltages_variable = 1;
    static constexpr int num_voltages_constant = 1;
    static constexpr int num_voltages = num_voltages_variable + num_voltages_constant;
    static constexpr int num_outputs = 1;

    // State Variables
    std::array<Eigen::Vector<T, num_states>, MAX_NUM_CHANNELS> x_n;
    std::array<Eigen::Vector<T, num_nl_ports>, MAX_NUM_CHANNELS> v_n;

    // NDK Matrices
    Eigen::Matrix<T, num_states, num_states> A_mat;
    Eigen::Matrix<T, num_states, num_voltages_variable> B_mat_var;
    Eigen::Vector<T, num_states> B_u_fix;
    Eigen::Matrix<T, num_states, num_nl_ports> C_mat;
    Eigen::Matrix<T, num_outputs, num_states> D_mat;
    Eigen::Matrix<T, num_outputs, num_voltages_variable> E_mat_var;
    Eigen::Vector<T, num_outputs> E_u_fix;
    Eigen::Matrix<T, num_outputs, num_nl_ports> F_mat;
    Eigen::Matrix<T, num_nl_ports, num_states> G_mat;
    Eigen::Matrix<T, num_nl_ports, num_voltages_variable> H_mat_var;
    Eigen::Vector<T, num_nl_ports> H_u_fix;
    Eigen::Matrix<T, num_nl_ports, num_nl_ports> K_mat;

    // Intermediate matrices for NDK matrix updates;
    Eigen::Matrix<T, num_pots, num_pots> Q;
    Eigen::Matrix<T, num_states, num_pots> Ux;
    Eigen::Matrix<T, num_outputs, num_pots> Uo;
    Eigen::Matrix<T, num_nl_ports, num_pots> Un;
    Eigen::Matrix<T, num_voltages, num_pots> Uu;
    Eigen::Matrix<T, num_states, num_states> A0_mat;
    Eigen::Matrix<T, num_states, num_voltages> B0_mat;
    Eigen::Matrix<T, num_states, num_nl_ports> C0_mat;
    Eigen::Matrix<T, num_outputs, num_states> D0_mat;
    Eigen::Matrix<T, num_outputs, num_voltages> E0_mat;
    Eigen::Matrix<T, num_outputs, num_nl_ports> F0_mat;
    Eigen::Matrix<T, num_nl_ports, num_states> G0_mat;
    Eigen::Matrix<T, num_nl_ports, num_voltages> H0_mat;
    Eigen::Matrix<T, num_nl_ports, num_nl_ports> K0_mat;
    Eigen::Matrix<T, num_states, num_states> two_Z_Gx;

    void reset_state();
    void reset (T fs);
    void update_pots (const std::array<T, num_pots>& pot_values);
    void process (std::span<T> channel_data, size_t channel_index) noexcept;
};
