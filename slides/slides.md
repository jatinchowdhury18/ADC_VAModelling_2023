---
title: 'ADC23 - VA Modelling'
# try also 'default' to start simple
theme: seriph
# apply any windi css classes to the current slide
class: 'text-center'
# https://sli.dev/custom/highlighters.html
highlighter: shiki
# show line numbers in code blocks
lineNumbers: false
favicon: 'https://chowdsp.com/logo.png'
# some information about the slides, markdown enabled
info: |
  ## ADC Virtual Analog Modelling 2023
# persist drawings in exports and build
drawings:
  persist: false
# use UnoCSS
css: unocss

layout: image
image: /Background.jpeg # @TODO: replcae the cover image
---

<!-- COVER PAGE -->

---

# Hello!

<div grid="~ cols-2 gap-4">
<div>

- I am a freelance audio DSP developer based in Seattle, WA
- Formerly at CCRMA and Tesla
- I spend lots of time making plugins:
  - [Chow Tape Model](https://github.com/jatinchowdhury18/AnalogTapeModel)
  - [BYOD](https://github.com/Chowdhury-DSP/BYOD)
  - [Chow Matrix](https://github.com/Chowdhury-DSP/ChowMatrix)
</div>
<div>
<img src="/headshot.jpg"/>
</div>
</div>

---
src: ./pages/intro.md
hide: false
---

---
src: ./pages/nodal.md
hide: false
---

---
src: ./pages/wdf.md
hide: false
---

---
src: ./pages/blackbox.md
hide: false
---

---

# Trying some features

$$
\begin{array}{c}

\nabla \times \vec{\mathbf{B}} -\, \frac1c\, \frac{\partial\vec{\mathbf{E}}}{\partial t} &
= \frac{4\pi}{c}\vec{\mathbf{j}}    \nabla \cdot \vec{\mathbf{E}} & = 4 \pi \rho \\

\nabla \times \vec{\mathbf{E}}\, +\, \frac1c\, \frac{\partial\vec{\mathbf{B}}}{\partial t} & = \vec{\mathbf{0}} \\

\nabla \cdot \vec{\mathbf{B}} & = 0

\end{array}
$$

---

<br/>

# Thanks!
<br/>

[jatin@chowdsp.com](mailto:chowdsp@gmail.com)

[https://github.com/jatinchowdhury18](https://github.com/jatinchowdhury18)

---
layout: image
image: /end_screen.jpeg # @TODO: replcae the cover image
---
