# Wave Digital Filters

Previously, at ADC 2020:

<v-clicks depth="2">

- Traditional Wave Digital Filters (WDFs)
  - Allow digital models of individual circuit components to be connected in the digital domain.
  - Require circuit to be de-composable into series/parallel junctions (no "multi-port" elements).
  - Require that circuit contain no more than one non-linear element.

</v-clicks>

<v-clicks at="4">

<div>
<br/>
<img src="/wdf_diagrams.png" style="margin-left:auto;margin-right:auto;width:75%">

</div>
</v-clicks>

---

# R-Type Adaptors

<v-clicks depth="2">

- In the 2010's [Kurt Werner](https://stacks.stanford.edu/file/druid:jy057cz8322/KurtJamesWernerDissertation-augmented.pdf) (and others) introduced the "R-Type Adaptor".
- Instead of trying to decompose the circuit into a Series/Parallel tree, use a [SPQR tree](https://en.wikipedia.org/wiki/SPQR_tree):
  - S: Series
  - P: Parallel
  - Q: (don't worry about it)
  - R: Rigid
- Each WDF adaptor is a "scattering matrix" that "routes" waves between the ports.
- The scattering matrix coefficients are determined by the impedances of the ports "facing" the adaptor.
- For most adaptors, there is one port impedance which can be chosen to make that port "reflection-free".
  - In practice, this is chosen to make on of the diagonal values in the scattering matrix equal to 0.
- Series/parallel adaptors can be re-used for every series/parallel topology.
- R-Type adaptors need to be re-derived for each circuit topology.

</v-clicks>

---

# Overdrive Circuit R-Type Adaptor

Label Ports (facing the R-Type Adaptor):

<img src="/ts_schematic_wdf_ports.svg" style="margin-left:auto;margin-right:auto">

---

# Overdrive Circuit R-Type Adaptor

Label Nodes (between the ports):

<img src="/ts_schematic_wdf_nodes.svg" style="margin-left:auto;margin-right:auto">

---

# Overdrive Circuit R-Type Adaptor

<div class="grid grid-cols-[55%_45%] gap-2">
<div>

Construct a netlist for the R-Type Adaptor:
- Replace each port with a Thevenin equivalent circuit.
  - Thevenin: voltage source + series resistor.
- Example:
  - Port A goes from node 3 → 4

```txt
Va 3 x # C4, DP, R6+P1
Ra 4 x
```

</div>
<div>

<img src="/ts_schematic_wdf_ports.svg" style="margin-left:auto;margin-right:auto">
<img src="/ts_schematic_wdf_nodes.svg" style="margin-left:auto;margin-right:auto">

</div>
</div>

---

# Overdrive Circuit R-Type Adaptor

<div class="grid grid-cols-[55%_45%] gap-2">
<div>

Construct a netlist for the R-Type Adaptor:
- Replace each port with a Thevenin equivalent circuit.
- Example:
  - Port A goes from node 3 → 4
  - Add ports B (1 → 2), C (1 → 3), D (1 → 4)

```txt {2-4,6-8}
Va 3 x # C4, DP, R6+P1
Vb 1 x # Vin, C2, R5
Vc 1 x # R4, C3
Vd 1 x # RL
Ra 4 x
Rb 2 x
Rc 3 x
Rd 4 x
```


</div>
<div>

<img src="/ts_schematic_wdf_ports.svg" style="margin-left:auto;margin-right:auto">
<img src="/ts_schematic_wdf_nodes.svg" style="margin-left:auto;margin-right:auto">

</div>
</div>

---

# Overdrive Circuit R-Type Adaptor

Construct a netlist for the R-Type Adaptor:
- Replace each port with a Thevenin equivalent circuit.
- Example:
  - Port A goes from node 3 → 4
  - Add ports B (1 → 2), C (1 → 3), D (1 → 4)
  - Add "virtual" nodes between each voltage source/resistor pair (A → 5, B → 6, C → 7, D → 8)

```txt
Va 3 5 # C4, DP, R6+P1
Vb 1 6 # Vin, C2, R5
Vc 1 7 # R4, C3
Vd 1 8 # RL
Ra 4 5
Rb 2 6
Rc 3 7
Rd 4 8
```

---

# Overdrive Circuit R-Type Adaptor

<div class="grid grid-cols-[55%_45%] gap-2">
<div>

Construct a netlist for the R-Type Adaptor:
- Replace each port with a Thevenin equivalent circuit.
- Example:
  - Port A goes from node 3 → 4
  - Add ports B (1 → 2), C (1 → 3), D (1 → 4)
  - Add "virtual" nodes
  - Add Op-Amp nodes

```txt {9}
Va 3 5 # C4, DP, R6+P1
Vb 1 6 # Vin, C2, R5
Vc 1 7 # R4, C3
Vd 1 8 # RL
Ra 4 5
Rb 2 6
Rc 3 7
Rd 4 8
E1 2 3 4 1 Ag # U1 (plus, minus, out, ground)
```

</div>
<div>

<img src="/ts_schematic_wdf_ports.svg" style="margin-left:auto;margin-right:auto">
<img src="/ts_schematic_wdf_nodes.svg" style="margin-left:auto;margin-right:auto">

</div>
</div>

---

# Overdrive Circuit R-Type Adaptor

Derive R-Type scattering matrix from the netlist:
- Make port A reflection-free since that port contains the diode-pair (nonlinear element).

<div grid="~ cols-2 gap-4">
<div>

Python + SageMath

<a href="https://github.com/jatinchowdhury18/R-Solver">
<img src="/r_solver_py.png"/>
</a>

</div>
<div>

Mathematica

<a href="https://www.wolframcloud.com/obj/chowdsp/Published/r_solver.nb">
<img src="/r_solver_math.png" style="height:75%"/>
</a>

</div>
</div>

---

# Overdrive Circuit R-Type Adaptor

```cpp {all}
// scattering matrix
const auto scattering_matrix[][] = {
    { 0, (Ag + Rd * Ri - Rc + Rd + Rc + Ro) / ((Rb + Rc) * Rd + ... },
    { -(Rb * Rc * Rd - Rd * Rc * Ro) / ((Ag + 1) * Rc * Rd * Ri + ... },
    { -(Rb * Rc * Rd + Rc * Rd * Ri - (Rb  * Rc + Rc * Ri) * Ro) / ... },
    { (Ag + Rc * Rd * Ri - ((Rb + Rc) * Rd + Rd * Ri) + Ro / ... }
};

// adapted port impedance
const auto Ra = ((Ag + 1) * Rc * Rd * Ri + Rb * Rc * Rd - (Rb * Rc + (Rb + Rc) * Rd + ...;
```

---

# Overdrive Circuit WDF Implementation

<v-clicks depth="3">

- [RT-WDF](https://github.com/RT-WDF/rt-wdf_lib) (2016): C++, circuit model defined at run-time
- [WDModels](https://faustlibraries.grame.fr/libs/wdmodels/) (2021): Faust, circuit model defined at compile-time
- [chowdsp_wdf](https://github.com/Chowdhury-DSP/chowdsp_wdf) (2022): C++, circuit model defined at either run-time or compile-time

</v-clicks>

<v-clicks at="3">

<div>
<br/>
<hr/>

Benchmark: time needed to process 1000 seconds of audio at 48 kHz sample rate:

|               |     `chowdsp_wdf` (compile-time)     | `chowdsp_wdf` (run-time) |              `wd_models`             | `RT-WDF` |
| :------------ |          :-----------:               | :-----------------: |              :---------:             | :------: |
| LPF-2         | <span style="color:red">0.434</span> |        2.763        |                  0.797               |  2.959   |
| FF-2          |             2.763                    |        6.127        | <span style="color:red">2.126</span> |  11.73   |
| Diode Clipper | <span style="color:red">2.041</span> |        2.091        |                  7.805               |   N/A    |
| Bassman Tone  | <span style="color:red">0.705</span> |        2.849        |                  0.770               |  7.978   |

</div>
</v-clicks>

---

# Overdrive Circuit WDF Implementation

```cpp
#include <chowdsp_wdf/chowdsp_wdf>
namespace wdft = chowdsp::wdft;

struct Overdrive_WDF
{
    // Port B
    wdft::CapacitiveVoltageSourceT<float> Vin_C2 { 1.0e-6f };                       // Vin + C2 in series
    wdft::ResistiveVoltageSourceT<float> R5 { 10.0e3f };                            // R5 + 4.5 V
    wdft::WDFParallelT<float, decltype (Vin_C2), decltype (R5)> P1 { Vin_C2, R5 };  // Parallel combination
    
    // Port C
    wdft::ResistorCapacitorSeriesT<float> R4_ser_C3 { 4.7e3f, 0.047e-6f }; // R4 + C3 in series

    // Port D
    wdft::ResistorT<float> RL { 1.0e6f };
};
```

---

# Overdrive Circuit WDF Implementation

```cpp {all} {maxHeight:'95%'}
struct Overdrive_WDF
{
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
            R.setSMatrixData ({
                { 0, (Ag + Rd * Ri - Rc + Rd + Rc + Ro) / ((Rb + Rc) * Rd + ... },
                { -(Rb * Rc * Rd - Rd * Rc * Ro) / ((Ag + 1) * Rc * Rd * Ri + ... },
                { -(Rb * Rc * Rd + Rc * Rd * Ri - (Rb  * Rc + Rc * Ri) * Ro) / ... },
                { (Ag + Rc * Rd * Ri - ((Rb + Rc) * Rd + Rd * Ri) + Ro / ... }
            });
            
            // adapted port impedance
            const auto Ra = ((Ag + 1) * Rc * Rd * Ri + Rb * Rc * Rd - (Rb * Rc + (Rb + Rc) * Rd + ...;
            return Ra;
    };
    
    
    using RType = wdft::RtypeAdaptor<float, 0, ImpedanceCalc, decltype (P1), decltype (R4_ser_C3), decltype (RL)>;
    RType R { P1, R4_ser_C3, RL };
};
```

---

# Overdrive Circuit WDF Implementation

<img src="/ts_wdf_plot.png" style="margin-left:auto;margin-right:auto;height:90%">

---

# Wave Digital Filters

<v-clicks>

- Can be extended to handle all arbitrary linear circuit topologies with R-Type adaptors.
- Can resolve up to one nonlinear element without requiring an iterative solver.
- Circuits with multiple/multi-port nonlinear elements typically require an iterative solver.

</v-clicks>
