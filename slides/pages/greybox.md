---
class: flex justify-center items-center
---

# Greybox Models

---

# Some Brief Preamble

<span class="text-sm">


- Usual Argument: Blackbox vs Whitebox --> Greybox

- Differentiable Digital Signal Processing 
    - DDSP: DIFFERENTIABLE DIGITAL SIGNAL PROCESSING (Jesse Engel, Lamtharn Hantrakul, Chenjie Gu, & Adam Roberts)-ICLR 2020

- Symbolic Regression - PySR: High-Performance Symbolic Regression in Python and Julia
##### Our Work
- Differentiable Wave Digital Filters (2022) SMC22 & ADC22
- Physics-Approximating Neural Networks (2022-23) ACM RACS22 & AES23 Europe
- WAFFLE (2023)

<img src="/explaingraph2.png" style="margin-left:auto;margin-right:auto;width:60%">

</span>

---

# Symbolic Regression with PySR


<div grid="~ cols-2 gap-4">
<div>
<video src="https://user-images.githubusercontent.com/7593028/238191929-c8511a49-b408-488f-8f18-b1749078268f.mp4" data-canonical-src="https://user-images.githubusercontent.com/7593028/238191929-c8511a49-b408-488f-8f18-b1749078268f.mp4" controls="controls" muted="muted" class="d-block rounded-bottom-2 border-top width-fit" style="max-height:300px; min-height: 100px">

  </video>

Credit: [github:MilesCranmer/PySR](https://github.com/MilesCranmer/PySR)


</div>

<div>

<span class="text-sm">

- **Symbolic Regression (SR):** A machine learning approach to find the best mathematical model for a dataset.
- **Optimization Goals:**
  - **Accuracy:** Prioritizes the precision of the model's fit to the data.
  - **Simplicity:** Aims to keep the mathematical expression as simple as possible.
- **Model Initialization:**
  - Does not require a pre-specified model structure.
  - Begins with initial expressions created by randomly combining:
    - Mathematical operators (e.g., +, -, *, /)
    - Functions (e.g., sin, cos, exp)
    - Constants (e.g., π, e)
    - Variables (representing data features)
- **Search Process:** Employs a stochastic approach to evolve and select the model that best captures the underlying data patterns.

</span>

</div>
</div>


---

# Results from Symbolic Regression

<span class="text-sm">

input = $\{x1, ... xN; \theta\}$ where $x_t = x1$, $x_{t-N} = xN$ , $\theta$  = control param.

we used that $N = 8$

$x_{out} = tanh(x3 - erf(cube(sin((atanh_{clip}(x8) * 0.60346466) - ceil(sign(atanh_{clip}(cube(erf(x2 - x9)))))))))$

$atanh_{clip} = atanh((x+1)\%2 - 1)$

</span>

<!-- atin plot goes here -->

---

### Differentiable Wave Digital FIlters
<div grid="~ cols-2 gap-4">
<div>

- As shown earlier, a diode clipper with a RC filter circuit can be represented as such: 
<img src="/wdfdiode.jpeg" style="margin-left:auto;margin-right:auto;width:65%">

- This WDF model can be embedded within a model training graph as shown on the right.

</div>

<div>
<img src="/modelgraph.png" style="margin-left:auto;margin-right:auto;width:125%">

</div>
</div>

---

- Covered in: Pipeline for VA Modelling with Physics-Informed Machine Learning 

<img src="/ADC2022-dwdf.png" style="margin-left:auto;margin-right:auto;height:70%"/>

---

# Overdrive Circuit DWDF Implementation

<img src="/ts_dwdf_plot.png" style="margin-left:auto;margin-right:auto;height:80%">

The difference in the plot comes from the previous neural network trained on real-world diodes.

---

<div grid="~ cols-2 gap-4">
<div>

## DWDF Counterfactual Information

Premise: Designer can choose what sort of neural network to model given nonlinearity

(approach with rough understanding)
- Recurrence to model time-based nonlinearity
- Convolution for "state-spatial" interactions
- Etc 

This means that we are able to learn more about the system based on the neural network that best resolves

</div>

<div>

<img src="/dwdf-neuralnetworkchoice.png"/>

The reverse is also true:

If neural network resolves nonlinearity;

Nonlinearity is modelled well by neural network;

Neural network architecture models a certain type of nonlinearity;

Nonlinearity is of that particular type.

</div>
</div>

---

# Physics-Approximating Neural Networks

<div grid="~ cols-2 gap-4">
<div>
Motivation

- We tend to think of neural networks as function approximators (under the universal approximation theorem - Hornik, Kurt; Stinchcombe, Maxwell; White, Halbert (1989)) 

- in reality, this is 99% <s>true</s>, only if the function is memoizable 

    - e.g non time-invariant functions not memoizable

    - e.g function is trained in one discretization spacing, but inferenced in another

</div>
<div>

<img src="/set2set.png" style="margin-left:auto;margin-right:auto;width:65%"/>

Since each set of hidden variables $H$ and hidden states $S$ will cause the function we intend to model $F$ to alter slightly, the function $F$ can be represented as carrying/containing a family of functions $\mathcal{F}$

<img src="/pann_motivation.png" style="margin-left:auto;margin-right:auto;width:65%"/>


</div>
</div>


---

<span class="text-sm">

# Physics-Approximating Neural Networks

- The neural network allows us to model each $f$ in family of functions $\mathcal{F}$ as a self-optimizing branch of the neural network.
- This way, defining the operator structure of each $f$ will act as a regularization of the neural network 

    - More well-defined the operator structure $\approx$ more regularization.

</span>
<div grid="~ cols-2 gap-4">
<div>

<img src="/capacitor.png" style="margin-left:auto;margin-right:auto;width:65%"/>

</div>
<div>

Van Maanen and van der Veen nonlinear capacitor model:
$V^2_t = \frac{2x^2_t.(m\ddot{x}+k_1\dot{x}+k_2[x_t-x_0])}{\in_C.A}$

Converted to operator structure:

$\mathbb{F}* = \frac{2 \mathbf{x_1}^2.(\mathbf{x_2}\mathbf{x_3}+\mathbf{x_4}\mathbf{x_5}+\mathbf{x_6}[\mathbf{x_7}-\mathbf{x_8}])}{\mathbf{x_9}.\mathbf{x_10}}$

where each $\mathbf{x_n}$ is a trainable variable based on input from previous layer.

</div>
</div>

---

# Physics-Approximating Neural Networks 

<img src="/PANN-results.png" style="margin-left:auto;margin-right:auto;width:80%"/>
The PANN model was trained alongside a Dense Model made of only densely-connected layers. This dense model was the same size as the PANN at 8 x 8. A linear model was also used to compare the results.

# Note on Counterfactual Information
Because we are allowed to tweak the operator structure of the model, this provides more explainability using discoverability of latent strcture. Side note on Genetic Algorithms.


---

# WAFFLE

Wave-digital-domain Autoencoder-shaped networks For Fast Learning and Emulation

<div grid="~ cols-2 gap-4">
<div>

<img src="/waffle-diagram.png" style="margin-left:auto;margin-right:auto;width:75%"/>

</div>
<div>
<span class="text-sm">
Consider a neural network with inputs:

$\text{R}_{\text{in}}$ : Voltages across potentiometer terminals that determine $\text{R}_{\text{in}}$. $\text{R}_{\text{in}-}$ : Resistance of the potentiometer minus $\text{R}_{\text{in}}$.

$C_7$,$C_8$ : The capacitance of the capacitor $C_7$,$C_8$.

$R_{13}$ : The resistance of the resistor $R_{13}$.

Concatenated to form a single input vector to the mesa-model $\mathbb{M}_{\texttt{circuit}}$.

$\mathbb{M}_{\texttt{circuit}} = \texttt{MLP} \circ \bigcup(\text{R}_{\text{in}},\text{R}_{\text{in}-},C_7 \bigcup C_8, R_{13})$

$x_{\texttt{latent}} = \bigcup(\mathbb{M}_{\texttt{circuit}},V_{\text{in}})$

$V_{\text{in}}$ is the transformed wave domain equivalent of the input signal.

</span>
</div>
</div>

---

# WAFFLE

Total error $1.07e-7$ trained for 150 epochs

<div grid="~ cols-2 gap-4">
<div>

<img src="/portgradient_colour.png" style="margin-left:auto;margin-right:auto;width:95%"/>

</div>
<div>
<span class="text-sm">

The output of $\mathbb{M}_{\text{PORTS}}$ is then concatenated with the output of the PANN model ($\hat{P}$) to form a single vector $\hat{y}_{ \texttt{latent}}$. $\hat{y}_{ \texttt{latent}}$ is then passed to a postprocessing MLP, $\mathbb{M}_{\text{post-processing}}$ to form the final output of the WAFFLE model.

$\mathbb{M}_{\text{PORTS}} = \texttt{MLP}([1.0] \ \forall \text{Port} \  \in  \ \text{Ports})$

where $i\text{Port} \in \text{Ports}\{A, B, C, D\}$.

$\hat{y} = \mathbb{M}_{\text{post-processing}} (\hat{y}_{ \texttt{latent}})$

$\hat{y} = \mathbb{M}_{\text{post-processing}} \circ \bigcup(\mathbb{M}_{\text{PORTS}},\hat{P})$

</span>
</div>
</div>

