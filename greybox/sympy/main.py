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


def add_timesteps(data, num_timesteps):
    """
    Adds columns to the data for previous timesteps.

    Parameters:
    - data: The original data array.
    - num_timesteps: The number of timesteps to include.

    Returns:
    - A new array with the additional timestep columns.
    """
    # Create a new array to hold the data with additional timesteps
    num_rows = data.shape[0]
    num_cols = data.shape[1] + num_timesteps
    new_data = np.zeros((num_rows, num_cols))

    # Fill in the original data
    new_data[:, : data.shape[1]] = data

    # Add the previous timesteps
    for i in range(1, num_timesteps + 1):
        # Shift data by i places, padding with zeros
        new_data[i:, data.shape[1] + i - 1] = data[:-i, 0]

    return new_data


# Step 1: Read and Process Audio Files
# Define your filenames
filenames = [
    "../../training/data/out_03.wav",
    "../../training/data/out_00.wav",
    "../../training/data/out_01.wav",
    "../../training/data/.DS_Store",
    "../../training/data/out_10.wav",
    "../../training/data/out_07.wav",
    "../../training/data/train_input_96k.wav",
    "../../training/data/out_001.wav",
    "../../training/data/out_0001.wav",
]

# Filter out .DS_Store and sort the list for consistency
wav_files = sorted(
    [
        file
        for file in filenames
        if file.endswith(".wav")
        and file != "../../training/data/train_input_96k.wav"
    ]
)

# Read the input signal
input_rate, input_signal = read("../../training/data/train_input_96k.wav")

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

# Use the function to add timesteps
num_timesteps = 8  # Or any other integer you want to specify
extended_data_array = add_timesteps(data_array, num_timesteps)

print(extended_data_array.shape)
print(extended_data_array[10000:10005, :])

# export extended_data_array to csv
# np.savetxt(
#     "blackbox/ADC_Training_Data/extended_data_array.csv",
#     extended_data_array,
#     delimiter=",",
# )

# Now, prepare the data for PySR
X = np.concatenate(
    (extended_data_array[:, :2], extended_data_array[:, 3:]), axis=1
)  # input features, with additional timesteps
y = extended_data_array[:, 2]  # output feature, same as before

print(X.shape)

exit()

# Step 3: Run PySR
# Initialize PySR model with desired options
model = PySRRegressor(
    niterations=10,
    population_size=30,
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
