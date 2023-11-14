#!/bin/bash

export_dir="slides/ADC_VAModelling_2023"

npx slidev build --base /~jatin/${export_dir}

ssh ccrma-gate.stanford.edu "rm -R Library/Web/${export_dir}"
scp -r dist ccrma-gate.stanford.edu:~/Library/Web/${export_dir}

ssh ccrma-gate.stanford.edu "cd Library/Web/slides && bash .gen-tree.sh"
