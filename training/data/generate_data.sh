#!/usr/bin/env bash

ROOT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )/../..
cd ${ROOT_DIR}

generator_exe="${ROOT_DIR}/cpp/build/data_generator/Debug/data_generator"
in_file="${ROOT_DIR}/training/data/train_input_mono_96k.wav"

## declare an array variable
declare -a dist_values=("0.0" "0.0001" "0.003" "0.0055" "0.01" "0.03" "0.55" "0.1" "0.17" "0.3" "0.4" "0.55" "0.65" "0.75" "1.0")
echo "Generating data for ${#dist_values[@]} parameter values"

## now loop through the above array
for dist_value in "${dist_values[@]}"
do
   echo "Generating output for distortion value: ${dist_value}"
   dist_label=$(echo "$dist_value" | tr . _)
   out_file="${ROOT_DIR}/training/data/train_output_${dist_label}.wav"
   $generator_exe $in_file $out_file $dist_value
done
