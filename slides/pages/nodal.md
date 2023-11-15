# Nodal Analysis

Previously, at ADC 2020:

<v-clicks depth="2">

- Laplace transformations
- Linear transfer functions (s-domain and z-domain)
- Bilinear transform
- Limitation: only works for linear circuits
- Alternative Methods:
  - Nodal DK (NDK)
  - Modified Nodal Analysis (MNA)

</v-clicks>
  
---

# Modified Nodal Analysis

Review of circuit rules:

<v-clicks>

$$ \text{Ohm's Law: } v_R = i_R R $$
$$ \text{Kirchoff's Current Law (KCL): } \sum_{k=1}^n i_k = 0 $$
$$ \text{Capacitor Voltage Law: } v_C(t) = \frac{Q_C(t)}{C} = \frac{1}{C} \int i_C(t) dt $$
$$ \text{ Ideal Op-Amp Laws: } v^+ = v^-; \; i^+ = i^- = 0  $$

</v-clicks>

---

# Modified Nodal Analysis

[Capacitor discretization](https://cytomic.com/files/dsp/OnePoleLinearLowPass.pdf):

<v-clicks>

$$ \text{Trapezoidal Rule: } \int_a^b f(x) dx \approx (b-a) * \frac{1}{2} (f(b) + f(a)) $$
$$ v_C(t) = \frac{1}{C} \int i_C(t) dt \rarr v_C[n] = v_C[n-1] + \frac{T}{C} * \frac{1}{2} (i_C[n] + i_C[n-1]) $$
$$ \text{Capacitor ``admittance'': } G_C = \frac{2 C}{T} = 2 f_s C \\
\text{Capacitor ``state'': } i_{C_{eq}}[n] = G_C v_C[n] + i_C[n] $$

<!-- <div>

- $G_C$ is more of an "algebraic" admittance than a "physical" admittance.
- $i_{c_{eq}}$ can be thought of as an added current due to the capacitor charge from the previous sample.

</div> -->

<div>
<br/>

<div style="border:2px solid #FFFFFF">

$$ \text{Voltage-Current relation: } i_C[n] = G_C v_C[n] - i_{C_{eq}}[n-1] $$
$$ \text{State update: } i_{C_{eq}}[n] = 2 G_C v_C[n] - i_{C_{eq}}[n-1] $$

</div>
</div>

</v-clicks>

---

# Modified Nodal Analysis

<div class="grid grid-cols-2 gap-2">
<div>
<img src="/mna_subschematic_C2R5.svg"/>
<v-clicks>

$$ i_{R5} = \frac{v^+ - v_b}{R_5} $$
$$ i_{C2} = 2 f_s C_2 (v_i - v^+) - i_{C2_{eq}} $$
$$ i_{C2} - i_{R5} = 0 \text{ (KCL)} $$

<div>

Solve for $v^+$ in terms of $v_i$
$$ v^+ = \frac{2 f_s C_2 R_5 v_i - R_5 i_{C2_{eq}} + v_b}{1 + 2 f_s C_2 R_5} $$

</div>
</v-clicks>
</div>
<div>
<v-clicks>

```cpp
struct Overdrive_MNA {
    float gC2 = 2.0f * fs * C2;
    float gC2_R5_recip = 1.0f / (1.0f + gC2 * R5);
    float iC2eq = 0.0f;
    
    float process_sample (float Vi) {
        // compute Vplus from Vi
        const auto Vplus = (gC2 * R5 * Vi - R5 * iC2eq + Vb) * gC2_R5_recip;
        
        // update capacitor states
        const auto vC2 = Vi - Vplus;
        iC2eq = 2.0f * gC2 * vC2 - iC2eq;
        
        return Vplus;
    }
};
```

<div style="margin-top:-75%;z-index:2;position:relative">
<img src="/mna_vplus_plot.png" style="margin-left:auto;margin-right:auto;width=10%">
</div>

</v-clicks>
</div>
</div>

---

# Modified Nodal Analysis

<div class="grid grid-cols-2 gap-2">
<div>
<img src="/mna_subschematic_C3R4.svg" style="margin-left:auto;margin-right:auto"/>
<v-clicks>

$$ i_{C3} = 2 f_s C_3 (V^- - V_4) - i_{C3_{eq}} $$
$$ i_{R4} = i_{C3} = \frac{V_4 - 0}{R_4} $$

<div>

Solve for $i_{C3}$ in terms of $v^-$
$$ i_{C3} = \frac{2 f_s C_3 v^- - i_{C3_{eq}}}{1 + 2 f_s C_3 R_4} $$

</div>
</v-clicks>
</div>
<div>
<v-clicks>

```cpp
struct Overdrive_MNA {
    // ...
    float gC3 = 2.0f * fs * C3;
    float gC3_recip = 1.0f / gC3;
    float gC3_R4_recip = 1.0f / (1.0f + gC3 * R4);
    float iC3eq = 0.0f;
    
    float process_sample (float Vi) {
        // compute Vplus from Vi
        const auto Vplus = (gC2 * R5 * Vi - R5 * iC2eq + Vb) * gC2_R5_recip;
        
        const auto Vminus = Vplus; // (op-amp rule)
        
        // compute i_C3 (same as i_R4)
        const auto iC3 = (gC3 * Vminus - iC3eq) * gC3_R4_recip;
        
        // update capacitor states
        const auto vC2 = Vi - Vplus;
        iC2eq = 2.0f * gC2 * vC2 - iC2eq;
        const auto vC3 = (iC3 + iC3eq) * gC3_recip;
        iC3eq = 2.0f * gC3 * vC3 - iC3eq;
    }
};
```

</v-clicks>
</div>
</div>

---

# Modified Nodal Analysis

<div class="grid grid-cols-2 gap-2">
<div>

<v-clicks>

$$ i_{C3} = i_{RP} + i_{DP} + i_{C4} \text{ (KCL)} $$
$$ i_{DP} = 2 I_s * \sinh \left(\frac{v_{DP}}{v_T \eta} \right) \text{(Shockley diode model)} $$
$$ i_{C3} = \frac{v_{DP}}{R_6 + P_1} + 2 I_s * \sinh \left(\frac{v_{DP}}{v_T \eta} \right) + 2 f_s C_4 v_{DP} - i_{C4_{eq}} $$

Solve for $v_{DP}$ with a Newton-Raphson solver:
$$ F(v_{DP}) = \frac{v_{DP}}{R_6 + P_1} + 2 I_s * \sinh \left(\frac{v_{DP}}{v_T \eta} \right) + 2 f_s C_4 v_{DP} - i_{C4_{eq}} - i_{C3} = 0 $$
$$ F'(v_{DP}) = \frac{1}{R_6 + P_1} + \frac{2 I_s}{v_T \eta} * \cosh \left(\frac{v_{DP}}{v_T \eta} \right) + 2 f_s C_4 $$

$$ v_{DP}^{k+1} = v_{DP}^k - \frac{F(v_{DP})}{F'(v_{DP})} $$

</v-clicks>
</div>
<div>
<img src="/mna_subschematic_FB.svg" style="margin-left:auto;margin-right:auto;height:55%"/>
</div>
</div>

---

# Modified Nodal Analysis

```cpp
struct Overdrive_MNA {
    // ...
    float RP_recip = 1.0f / (R6 + 0.1f * P1);
    float gC4 = 2.0f * fs * C4;
    float iC4eq = 0.0f;
    float Vd = 0.0f;
    
    float process_sample (float Vi) {
        // ...
        int nIters = 0;
        float delta;
        do {
            const auto [sinh_Vd, cosh_Vd] = sinh_cosh (Vd * Vt_recip);
            const auto F = Vd * RP_recip + gC4 * Vd - iC4eq - iC3 + twoIs * sinh_Vd;
            const auto F_deriv = RP_recip + gC4 + (twoIs * Vt_recip) * cosh_Vd;
            delta = F / F_deriv;
            Vd -= delta;
        } while (std::abs (delta) > 1.0e-3f && ++nIters < 10);
        
        // update capacitor states
        // ...
        iC4eq = 2.0f * gC4 * Vd - iC4eq;
    }
};
```

---

# Modified Nodal Analysis Complete Implementation

```cpp
float process_sample (float Vi) {
    const auto Vplus = (gC2 * R5 * Vi - R5 * iC2eq + Vb) * gC2_R5_recip; // compute Vplus from Vi
    const auto Vminus = Vplus; // Vminus = Vplus (op-amp rule)
    const auto iC3 = (gC3 * Vminus - iC3eq) * gC3_R4_recip; // compute current through C3 (same as current through R4)

    int nIters = 0; T delta;
    do {
        const auto [sinh_Vd, cosh_Vd] = sinh_cosh (Vd * Vt_recip);
        const auto F = Vd * RP_recip + gC4 * Vd - iC4eq - iC3 + twoIs * sinh_Vd;
        const auto F_deriv = RP_recip + gC4 + (twoIs * Vt_recip) * cosh_Vd;
        delta = F / F_deriv;
        Vd -= delta;
    } while (std::abs (delta) > (T) 1.0e-3 && ++nIters < 10);

    // update capacitor states
    const auto vC2 = Vi - Vplus; iC2eq = (T) 2 * gC2 * vC2 - iC2eq;
    const auto vC3 = (iC3 + iC3eq) * gC3_recip; iC3eq = (T) 2 * gC3 * vC3 - iC3eq;
    iC4eq = (T) 2 * gC4 * Vd - iC4eq;

    return Vminus + Vd; // read output voltage
}
```

<v-clicks>

Exercise for the reader: can we use SIMD to optimize `processSample()`?

</v-clicks>

---

# Modified Nodal Analysis

<img src="/ts_mna_plot.png" style="margin-left:auto;margin-right:auto;height:90%">

---

# Nodal DK Method

<v-clicks depth="2">

- "Nodal Discrete K-Method"
- Introduced by [David Yeh, Jonathan Abel, and Julius Smith](https://ccrma.stanford.edu/~dtyeh/papers/yeh10_taslp.pdf) in the 2000's.
- Modified by [Martin Holters and Udo Zolzer](http://recherche.ircam.fr/pub/dafx11/Papers/21_e.pdf) (2011) to efficiently handle variable elements (potentiometers).
- System for creating automated NDK models presented at ADC 2019 by [Daniel Struebig](https://github.com/dstrub18/NDKFramework).

</v-clicks>

---

# Nodal DK Method

Constructing a netlist

<div class="grid grid-cols-[35%_65%] gap-2">
<div>

```txt
Vi 1 0
Vb 6 0 FIXED
R5 2 6
R4 4 0
RL 5 0
R6_P1 3 5 VARIABLE
C2 2 1
C3 3 4
C4 3 5
U1 2 3 5 # PLUS MINUS OUT
DP 3 5
```

</div>
<div>

<img src="/ts_schematic_ndk_nodes.svg" style="margin-left:auto;margin-right:auto">

</div>
</div>

---

# Nodal DK Method

Netlist to "layout" matrices (can be auto-generated):

<div class="grid grid-cols-[35%_65%] gap-2">
<div>

```txt
Vi 1 0
Vb 6 0 FIXED
R5 2 6
R4 4 0
RL 5 0
R6_P1 3 5 VARIABLE
C2 2 1
C3 3 4
C4 3 5
U1 2 3 5 # PLUS MINUS OUT
DP 3 5
```

</div>
<div>

```cpp{all|1-5|6-16|17-19|all}
const Eigen::Matrix<T, num_resistors, num_nodes> Nr { // resistor layouts
    { +0, +1, +0, +0, +0, -1 }, // R5 goes from node 2 → node 6
    { +0, +0, +0, +1, +0, +0 }, // R4 goes from node 4 → node 0
    { +0, +0, +0, +0, +1, +0 }, // RL goes from node 5 → node 0
};
const Eigen::Matrix<T, num_pots, num_nodes> Nv {  // potentiometer layouts
    { +0, +0, +1, +0, -1, +0 }
};
const Eigen::Matrix<T, num_states, num_nodes> Nx { // capacitor layouts
    { -1, +1, +0, +0, +0, +0 },
    { +0, +0, +1, -1, +0, +0 },
    { +0, +0, +1, +0, -1, +0 },
};
const Eigen::Matrix<T, num_nl_ports, num_nodes> Nn { // nonlinearity layouts
    { +0, +0, +1, +0, -1, +0 }
};
const Eigen::Matrix<T, num_outputs, num_nodes> No { // "output" node layouts
    { +0, +0, +0, +0, +1, +0 }
};
```
</div>
</div>

---

# Nodal DK Method

Compute state-space system matrices:

$$
S = \begin{pmatrix}
N_R^T G_R N_R + N_v^T R_V^{-1} N_v + N_x G_x N_x & N_u^T \\
N_u & 0
\end{pmatrix}
$$
$$
A = 2 Z G_x \begin{pmatrix} N_x & 0 \end{pmatrix} S^{-1} \begin{pmatrix} N_x & 0 \end{pmatrix}^T - Z \\
B = 2 Z G_x \begin{pmatrix} N_x & 0 \end{pmatrix} S^{-1} \begin{pmatrix} 0 & I \end{pmatrix}^T \\
C = 2 Z G_x \begin{pmatrix} N_x & 0 \end{pmatrix} S^{-1} \begin{pmatrix} N_n & 0 \end{pmatrix}^T \\
D = \begin{pmatrix} N_o & 0 \end{pmatrix} S^{-1} \begin{pmatrix} N_x & 0 \end{pmatrix}^T \\
E = \begin{pmatrix} N_o & 0 \end{pmatrix} S^{-1} \begin{pmatrix} 0 & I \end{pmatrix}^T \\
F = \begin{pmatrix} N_o & 0 \end{pmatrix} S^{-1} \begin{pmatrix} N_n & 0 \end{pmatrix}^T \\
G = \begin{pmatrix} N_n & 0 \end{pmatrix} S^{-1} \begin{pmatrix} N_x & 0 \end{pmatrix}^T \\
H = \begin{pmatrix} N_n & 0 \end{pmatrix} S^{-1} \begin{pmatrix} 0 & I \end{pmatrix}^T \\
K = \begin{pmatrix} N_n & 0 \end{pmatrix} S^{-1} \begin{pmatrix} N_n & 0 \end{pmatrix}^T
$$

---

# Nodal DK Method

Final state-space system:

<v-clicks at="0">
<div>

- $x$: state
- $u$: inputs
- $y$: outputs
- $i_n$, $v_n$: current and voltage across the nonlinear elements

</div>
</v-clicks>
<v-clicks at="1">
<div>

$$
\text{State update equation: } x[n] = A x[n-1] + B u[n] + C i_n[n]
$$
$$
\text{Output equations: } y[n] = D x[n-1] + E u[n] + F i_n[n]
$$
$$
\text{Nonlinear elements equation: } v_n[n] = G x[n-1] + H u[n] + K i_n[n]
$$

</div>
</v-clicks>

<v-clicks at="2">
<div>

<span style="color:red">
Solving for the nonlinear elements usually requires an iterative solver or a pre-computed lookup table.
</span>

</div>
</v-clicks>

---

# Nodal DK Method

```cpp{all|7-18|20-21|23-25|all}
const auto calc_currents = [&] {
    std::tie (sinh_v0, cosh_v0) = sinh_cosh (v_n[ch](0) / (n_DP1 * Vt));
    i_n (0) = (T) 2 * -Is_DP1 * sinh_v0;
};
const auto calc_jacobian = [&] { Jac (0, 0) = ((T) 2 * -Is_DP1 / (n_DP1 * Vt)) * cosh_v0; };

// compute nonlinear elements (Newton-Raphson)
p_n.noalias() = G_mat * x_n[ch] + H_mat_var * u_n + H_u_fix;
do {
    calc_currents();
    calc_jacobian();
    
    F_min.noalias() = p_n + K_mat * i_n - v_n[ch];
    A_solve.noalias() = K_mat * Jac - eye;
    delta_v.noalias() = A_solve.householderQr().solve (F_min);
    v_n[ch] -= delta_v;
} while (delta > 1.0e-3 && ++nIters < 10);
calc_currents();

// compute outputs
y_n.noalias() = D_mat * x_n[ch] + E_mat_var * u_n_var + E_u_fix + F_mat * i_n;

// compute state update
x_n[ch] = A_mat * x_n[ch] + B_mat_var * u_n_var + B_u_fix + C_mat * i_n;
```

---

# Nodal DK Method

Code generation from a JSON "configuration":

```json
{
    "struct_name": "TS_NDK",
    "netlist": [
        "Vi 1 0",
        "Vb 6 0 FIXED",
        "R5 2 6",
        "R4 4 0",
        "RL 5 0",
        "R6_P1 3 5 VARIABLE",
        "C2 2 1",
        "C3 3 4",
        "C4 3 5",
        "A1 2 3 5 # PLUS MINUS OUT",
        "DP1 3 5"
    ],
    "output_nodes": [ 5 ],
    "header_includes": [
        "#include <Eigen/Dense>"
    ],
    "nr_exit_condition": "delta > 1.0e-3 && ++nIters < 10"
}
```

---

# Nodal DK Method

<img src="/ts_ndk_plot.png" style="margin-left:auto;margin-right:auto;height:90%">

---

# Nodal Analysis

<v-clicks depth="2">

- Can be extended to nonlinear circuits using Nodal DK, Modified Nodal Analysis, and other techniques.
- Requires understanding of "traditional" circuit analysis.
- Often requires an iterative solver (or similar) to resolve nonlinear elements.

</v-clicks>
