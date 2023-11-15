
<span class="flex justify-center items-center">

# Blackbox Models

</span>

---

# Neural Network Architectures

<span class="text-sm">

| WaveNet | Temporal Convolutional Networks (TCNs) | Recurrent Neural Networks (RNNs) | State Transition Networks |
|---------|----------------------------------------|---------------------------------|---------------------------|
| A deep neural network for generating raw audio waveforms, known for its high fidelity in text-to-speech and musical applications. | A type of convolutional neural network designed to handle sequence data by capturing temporal dependencies. | Neural networks with loops to allow information persistence, suitable for handling sequence data like time-series or text. | Networks modeling the transitions between states in a system, often used to learn and represent dynamic behaviours. |
| - Utilizes causal dilated convolutions. <br> - Capable of modeling temporal dependencies in data. <br> - High-quality audio generation. | - Employs dilated convolutions for long-range dependencies. <br> - Efficient and parallelizable architecture. <br> - Often used in tasks like sequence modeling and time-series prediction. | - Processes sequences step-by-step, maintaining an internal state. <br> - Suited for tasks like language modeling, speech recognition. <br> - Variants include LSTM and GRU. | - Captures state transitions in a structured manner. <br> - Suitable for tasks with clear state dynamics. <br> - Can be integrated with other architectures for complex modeling. |

</span>

---

<span class="text-sm">

## ToneX by IK Multimedia
- Designed by IK Multimedia for creating, playing, and sharing Tone Models of amps, cabinets, combos, and pedal models.
- Utilizes AI Machine Modeling technology to model the sound of any amp, cabinet, combo or pedal with high sonic accuracy.
- A ToneX capture can be trained in just a few minutes.
- Network type unknown.

## BABY Audio TAIP
- Tape emulation VST plugin that brings the true sound and behavior of analog tape to DAWs.
- Network type unknown.

## Neural Amp Modeler
- Open Source guitar amp/pedal capture system and real-time plugin.
- Uses a convolutional network.

</span>

---

# Progress Update: Tackling Remaining Challenges

<span class="text-med">

### Challenges in 2020: Neural networks ...
- Are difficult to implement
- Are computationally expensive
- Have trouble with sample rate conversions (sometimes)
- Have trouble with control parameters

<br/>

### Solutions:
- Real-Time Inferencing Engine: [RTNeural](https://github.com/jatinchowdhury18/RTNeural)
  - (Relatively) easy to port models trained in TensorFlow or PyTorch.
  - [Highly performant](https://github.com/jatinchowdhury18/RTNeural-compare), especially when model architecture is defined at compile-time.
- [Recurrent networks can be adapted for multiple sample rates](https://medium.com/@jatinchowdhury18/sample-rate-agnostic-recurrent-neural-networks-238731446b2)
  - Only works if inferencing sample rate is greater than or equal to training sample rate.
- Networks can be trained to learn control parameters as conditioning inputs (see [Wright et al.](https://dafx.de/paper-archive/2019/DAFx2019_paper_43.pdf)).

</span>

---

# Overdrive Circuit RNN Implementation

<img src="/ts_rnn_plot.png" style="margin-left:auto;margin-right:auto;height:90%">

---
class: flex justify-center items-center
---

# LACTOSE

---

# Conditional Models & LACTOSE Algorithm
<div grid="~ cols-2 gap-4">
<div>

<span class="text-sm">

### Background/Overview
- Some ML methods do not require backpropagation (Markov Modelling, Decision Trees)
- Proposed methods
  - Conditional Modelling (CM) -  Conditional Random Fields (CRF)
  - Conditional Neural Processes (CNP) - similar to Gaussian Process/Bayesian Methods
  - Conditional VAEs | Conditional seq2seq

</span>
</div>
<div>

### Difficulties in Conditional Models

- Traditional neural networks struggle with conditional statements within the graph.
- Backpropagation is hindered by branching conditions.
- Conditional statements present unique challenges, especially in the circunmstances presented in audio.
</div> 
</div>

<img src="/conditionalinorout.png" style="margin-left:auto;margin-right:auto;width:50%">


---

<div grid="~ cols-2 gap-4">
<div>

# Addressing with LACTOSE
- Linear Array of Conditions, TOpologies with Separated Error-backpropagation (LACTOSE)
- LACTOSE enables differentiable "if" conditions, which can allow the use of these in various architectures.
- Employs a dynamic parameter loading strategy during prediction.
- In theory, this would allow each model to be smaller in size -- Needs to bound less parameter space

</div>

  <div>

<figure style="margin-left:auto;margin-right:auto;width:70%">
<!-- <img src="/ts_schematic_wdf_nodes.svg" style="margin-left:auto;margin-right:auto"> -->
    <img src="/CombinedParameterSpace.png" />
    <figcaption>A visualisation of model parameter space encompassing vs. separated conditions.</figcaption>
</figure>
    <img src="/SeparatedParameterSpace.png" style="margin-left:auto;margin-right:auto;width:70%">

  </div>
</div>


---


<div grid="~ cols-2 gap-4">
<div>

<span class="text-sm">

# LACTOSE Algorithm in Practice
- Overcomes issues with non-zero gradients in conditional branches.
- Conditions and model parameters are stored externally to the Tensorflow graph.

</span>

$$
\begin{equation}
    y = \begin{cases}
        0.25x - 0.25  & \text{if } x \leq -0.5         \\
        0.5x  - 0.125 & \text{if } -0.5 < x \leq -0.25 \\
        x             & \text{if } -0.25 < x \leq 0.25 \\
        0.5x + 0.125  & \text{if } 0.25 < x \leq 0.5   \\
        0.25x + 0.25  & \text{if } x > 0.5
    \end{cases}
\end{equation}
$$

Dataset was prepared 7.2e5 samples, LACTOSE model: 4x4, non-LACTOSE 8x8. 4x4 non-LACTOSE was not able to train. ReLU activation fn. 
</div>

<div>

<img src="/ModelV2.png" style="margin-left:auto;margin-right:auto;width:85%">

``` c
While Train==True:
  // ModelInput: x
  // ModelOutput: y
  IF x = C_n // check across all conditions
  RETURN θ_n
  ENDIF

  Model <-- θ_n
  Prediction ^y = Model(x, θ_n)
  Loss = LossFunction(y, ^y)
  Optimizer <-- Loss
  Save(ModelParams: C_n)

```
</div>

</div>

---

# LACTOSE Results & Computational Cost
<div grid="~ cols-2 gap-4">
<div>
<span class="text-sm">

- Evaluated on a dataset with a piecewise linear transfer function.
  - Noticed that this works best for static nonlinearities, in order to keep the model size small.

- 720,000 samples split into 80% training, 20% testing.
- Outperforms an MLP in efficiency with reduced parameter space.

| Model   | Total Duration   | Mean Duration/Samp. | Realtime Score   |
|---------|------------------|---------------------|------------------|
| LACTOSE | 1.87e-2 seconds  | 1.62e-10 seconds    | 535.48×          |
| MLP     | 3.80e-2 seconds  | 3.3e-10 seconds     | 263.00×          |


</span>
</div>

<div>
<img src="/modelcompareLACTOSE.png" style="margin-left:auto;margin-right:auto;width:95%">

This benchmark was produced on a 2018 Mac Mini, with a 3.2 GHz 6-core Intel Core i7 CPU with AVX instructions enabled (RTNeural).
</div>
</div>


---
class: flex justify-center items-center
---

# Transformer Model

---


<div grid="~ cols-3 gap-4">
<div>

## Transformer Model
```mermaid {scale: 0.5}
graph TD
    A[Input] -->|Linear Transformation| B[Linear Encoder]
    B -->|Adds Positional Info| C[Positional Encoding]
    C -->|Transforms Data| D[Transformer Encoder Layer 1]
    D --> E[Transformer Encoder Layer 2]
    E --> F[/.../]
    F --> G[Transformer Encoder Layer N]
    G -->|Linear Transformation| H[Linear Decoder]
    H -->|Calculates Loss| I[MSE Loss]
```
</div>

<div>

<span class="text-sm">

##### Linear Encoder
- A dense layer that projects the input features into a higher-dimensional space suitable for the transformer.

##### Positional Encoding (PE)
- Infuses the input with positional data to maintain sequence information.

##### Transformer Encoder Layers
- Comprised of N layers (in this case, 8), each transformer encoder layer further processes the data, applying self-attention mechanisms and feedforward neural networks.

##### Linear Decoder
- Converts the high-dimensional output of the last encoder layer back into the original feature space, in this case, predicting a single continuous value.

</span>

</div>

<div>

<span class="text-sm">

##### Sine-cosine PE
- Sine-cosine positional encoding gives unique positional identifiers to sequence elements using alternating sine and cosine functions, enabling a model to understand the order of inputs.

- Applied to audio, this allows the model to recognize the order and temporal markers associated to the audio samples.

</span>
</div>

</div>

---

## Transformer Results and Discussion
<span class="text-sm">

- Transformers are typically known for their generative ability. However, in the case of learning the mapping of the Tubescreamer clipping stage, the results show that more work was necessitated. The above image is a smaller transformer (2 enc layers, dim=4) and the bottom is a larger transformer (8 enc layers, dim=8)

</span>

<img src="/transformer_10epochs.png" style="margin-left:auto;margin-right:auto;width:45%">
<img src="/transformer.png" style="margin-left:auto;margin-right:auto;width:45%">

<span class="text-sm">

- These results indicate that a larger transformer model is required to model the underlying mapping of the clipping stage. Or datum that span a larger distribution of the required space.

- Note on Generative Ability: Although the replication of the TS clipping stage might not be as good as other models, it is possible that there are other latent abilities the transformer model is capable of, even at low parameter count.

</span>

---




