import numpy as np
from scipy.io.wavfile import read
from pysr import PySRRegressor
import re
import os

print(os.listdir("."))


# Function to extract the parameter value from the filename
def extract_param(filename):
    match = re.search(r"out_(\d+).wav", filename)
    if match:
        # Extract the numeric part and convert to a float divided by 100 to get the param value
        return float("0." + match.group(1).lstrip("0"))
    return None


# Step 1: Read and Process Audio Files
# Define your filenames
filenames = [
    "blackbox/ADC_Training_Data/out_03.wav",
    "blackbox/ADC_Training_Data/out_00.wav",
    "blackbox/ADC_Training_Data/out_01.wav",
    "blackbox/ADC_Training_Data/.DS_Store",
    "blackbox/ADC_Training_Data/out_10.wav",
    "blackbox/ADC_Training_Data/out_07.wav",
    "blackbox/ADC_Training_Data/train_input_96k.wav",
    "blackbox/ADC_Training_Data/out_001.wav",
    "blackbox/ADC_Training_Data/out_0001.wav",
]

# Filter out .DS_Store and sort the list for consistency
wav_files = sorted(
    [
        file
        for file in filenames
        if file.endswith(".wav")
        and file != "blackbox/ADC_Training_Data/train_input_96k.wav"
    ]
)

# Read the input signal
input_rate, input_signal = read("blackbox/ADC_Training_Data/train_input_96k.wav")

# Initialize an empty list to hold all data
data = []

# Process each output file
for file in wav_files:
    # Extract the param value from the filename
    param_value = extract_param(file)
    if param_value is not None:
        # Read the output signal
        output_rate, output_signal = read(file)
        # Ensure the input and output signals have the same length
        min_length = min(len(input_signal), len(output_signal))
        input_signal = input_signal[:min_length]
        output_signal = output_signal[:min_length]
        # Combine the data
        for t in range(min_length):
            data.append([input_signal[t], param_value, output_signal[t]])

# Convert to a NumPy array
data_array = np.array(data)

# Step 2: Prepare Data for PySR
X = data_array[:, :2]  # input features
y = data_array[:, 2]  # output feature

# Step 3: Run PySR
# Initialize PySR model with desired options
model = PySRRegressor(
    niterations=5,
    population_size=20,
    binary_operators=["*", "+", "-", "/"],
    unary_operators=[
        "cos",
        "exp",
        "sin",
        "log",
        "tanh",
        "square",
        "cube",
        "exp",
        "atanh_clip",
        "erf",
        "erfc",
        # "gamma",
        "relu",
        "round",
        "floor",
        "ceil",
        "round",
        "sign",
    ],
    batching=True,  # Enable batching
    batch_size=10000,  # Set the batch size as needed
)

# Fit the model
model.fit(X, y)

# Step 4: Analyze the Results
# Print the discovered equations
print(model)
