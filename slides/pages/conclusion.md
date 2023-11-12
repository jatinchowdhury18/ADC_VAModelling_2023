# Performance Comparison

<div grid="~ cols-2 gap-4">
<div>

```mermaid {scale: 0.6}
xychart-beta
    title "Performance"
    x-axis [WDF, MNA, RNN-12, RNN-24, DWDF, NDK]
    y-axis "Real-Time Factor" 10 --> 225
    bar [178.4, 204.7, 96.5, 41.1, 126.5, 109.9]
```

</div>
<div>
<v-clicks depth="2">

- White-box models can be _very_ fast.
- Black-box models can achieve good performance, but larger networks are slower.
- Grey-box models often fall somewhere in-between.
- YMMV depending on the circuit being modelled and the accuracy required.
- Trade-offs (Accuracy vs. speed):
  - Iterative solver constraints
  - Function approximations
  - Neural network size

</v-clicks>
</div>
</div>
