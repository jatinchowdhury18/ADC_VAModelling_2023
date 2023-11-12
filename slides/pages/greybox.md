---
class: flex justify-center items-center
---

# Greybox Models

---

# Greybox

<span class="text-sm">

- Talk about the spectrum of greybox models (include image describing explainability vs blackbox & abstraction vs less abstracted)

- Talk about DDSP briefly

- DWDF, PANN, WAFFLE, Symbolic Regression

</span>

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

## DWDF Counterfactual Information
<div grid="~ cols-2 gap-4">
<div>


</div>

<div>


</div>
</div>

---

# Overdrive Circuit DWDF Implementation

<img src="/ts_dwdf_plot.png" style="margin-left:auto;margin-right:auto;height:90%">