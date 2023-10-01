#!/bin/bash

export_dir="slides/RTNeural_NAP"

npx slidev build --base /~jatin/${export_dir}

ssh ccrma-gate.stanford.edu "rm -R Library/Web/${export_dir}"
scp -r dist ccrma-gate.stanford.edu:~/Library/Web/${export_dir}
