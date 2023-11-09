# %%
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import torch
import torchaudio
from tqdm import tqdm

# Get current path
import os

filename_to_save = "plot"
# Set path to ADC_VAModelling_2023
os.chdir("../..")
print(os.getcwd())  # turn off if running in terminal, only for jupyter interactive
# list files in dir
print(os.listdir())
inputfile_path = "blackbox/ADC_Training_Data/train_input_96k.wav"
outputfile_path = "blackbox/ADC_Training_Data/out_10.wav"
# %%
# read WAVE files as numpy array using torchaudio
input_tensor, fs = torchaudio.load(inputfile_path)
input_tensor_np = input_tensor.numpy()
output_tensor, _ = torchaudio.load(outputfile_path)
output_tensor_np = output_tensor.numpy()
# %%
testplot = input_tensor_np.flatten()
testplot2 = output_tensor_np.flatten()

plotlength = int(len(testplot) / 600)
# plot input against output first 10000 samples
for i in tqdm(range(600)):
    plt.plot(
        testplot[0:plotlength],
        testplot2[0:plotlength],
        "o",
        color="black",
        markersize=0.2,
        alpha=0.1,
    )
    plt.axis("square")
    # make axis in centre
    plt.axhline(y=0, color="k", linewidth=0.5)
    plt.axvline(x=0, color="k", linewidth=0.5)
    plt.xlim(-1, 1)
    plt.ylim(-1, 1)
    plt.title("Tubescreamer IO Transfer Function")
    plt.xlabel("Input")
    plt.ylabel("Output")
    plt.savefig(f"blackbox/LACTOSE/figures/{filename_to_save}" + str(i) + ".png")
    # plt.show()
    plt.close()
    plotlength = plotlength + int(len(testplot) / 600)

    # print(plotlength)
# %%
# create movie with images using ffmpeg, run command with exec, images are in blackbox/LACTOSE/figures/plot
import subprocess

# create movie with images using ffmpeg
command = f"ffmpeg -r 60 -i blackbox/LACTOSE/figures/{filename_to_save}%d.png -vcodec mpeg4 -y blackbox/LACTOSE/figures/{filename_to_save}.mp4"
subprocess.run(command, shell=True)

# %%
# create GIF with images using ffmpeg
# command = f"ffmpeg -r 60 -i blackbox/LACTOSE/figures/{filename_to_save}%d.png -y blackbox/LACTOSE/figures/{filename_to_save}.gif"
# subprocess.run(command, shell=True)

# %%
# delete plots after creating movie
command = f"rm blackbox/LACTOSE/figures/{filename_to_save}*.png"
subprocess.run(command, shell=True)

# %%
