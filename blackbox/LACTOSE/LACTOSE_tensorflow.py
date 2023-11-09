# %%
import tensorflow as tf
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.io import wavfile
from tqdm import tqdm
from sklearn.model_selection import train_test_split
import os

# Set path to ADC_VAModelling_2023
os.chdir("..")
print(os.getcwd())  # turn off if running in terminal, only for jupyter interactive
# list files in dir
print(os.listdir())

# Read data from wav files
sample_rate_input, input_data = wavfile.read("ADC_Training_Data/train_input_96k.wav")
sample_rate_output, output_data = wavfile.read("ADC_Training_Data/out_10.wav")

# Calculate the split index
split_idx = int(len(input_data) * 0.8)  # 80% for training, 20% for testing

# Split the data in a continuous way
input_train, input_test = input_data[:split_idx], input_data[split_idx:]
output_train, output_test = output_data[:split_idx], output_data[split_idx:]

# Now create conditional arrays using list comprehensions
# This will be much faster than appending in a loop
input_1_train = input_train[input_train < -0.5]
output_1_train = output_train[input_train < -0.5]

input_2_train = input_train[(input_train >= -0.5) & (input_train < -0.25)]
output_2_train = output_train[(input_train >= -0.5) & (input_train < -0.25)]

input_3_train = input_train[(input_train >= -0.25) & (input_train < 0.25)]
output_3_train = output_train[(input_train >= -0.25) & (input_train < 0.25)]

input_4_train = input_train[(input_train >= 0.25) & (input_train < 0.5)]
output_4_train = output_train[(input_train >= 0.25) & (input_train < 0.5)]

input_5_train = input_train[input_train >= 0.5]
output_5_train = output_train[input_train >= 0.5]

# Repeat the process for the test sets
input_1_test = input_test[input_test < -0.5]
output_1_test = output_test[input_test < -0.5]

input_2_test = input_test[(input_test >= -0.5) & (input_test < -0.25)]
output_2_test = output_test[(input_test >= -0.5) & (input_test < -0.25)]

input_3_test = input_test[(input_test >= -0.25) & (input_test < 0.25)]
output_3_test = output_test[(input_test >= -0.25) & (input_test < 0.25)]

input_4_test = input_test[(input_test >= 0.25) & (input_test < 0.5)]
output_4_test = output_test[(input_test >= 0.25) & (input_test < 0.5)]

input_5_test = input_test[input_test >= 0.5]
output_5_test = output_test[input_test >= 0.5]

# Print the shapes of the arrays to verify
print("Training Data Shapes:")
print(input_1_train.shape, output_1_train.shape)
print(input_2_train.shape, output_2_train.shape)
print(input_3_train.shape, output_3_train.shape)
print(input_4_train.shape, output_4_train.shape)
print(input_5_train.shape, output_5_train.shape)

print("\nTesting Data Shapes:")
print(input_1_test.shape, output_1_test.shape)
print(input_2_test.shape, output_2_test.shape)
print(input_3_test.shape, output_3_test.shape)
print(input_4_test.shape, output_4_test.shape)
print(input_5_test.shape, output_5_test.shape)
# %%

# train 5 models for each input_N to output_N
# create model
# %%
model_1 = tf.keras.Sequential()
model_1.add(tf.keras.layers.Dense(1, input_shape=(1,)))
model_1.add(tf.keras.layers.Dense(4, activation="tanh"))
model_1.add(tf.keras.layers.Dense(4, activation="tanh"))
model_1.add(tf.keras.layers.Dense(1))

model_2 = tf.keras.Sequential()
model_2.add(tf.keras.layers.Dense(1, input_shape=(1,)))
model_2.add(tf.keras.layers.Dense(4, activation="tanh"))
model_2.add(tf.keras.layers.Dense(4, activation="tanh"))
model_2.add(tf.keras.layers.Dense(1))

model_3 = tf.keras.Sequential()
model_3.add(tf.keras.layers.Dense(1, input_shape=(1,)))
model_3.add(tf.keras.layers.Dense(4, activation="tanh"))
model_3.add(tf.keras.layers.Dense(4, activation="tanh"))
model_3.add(tf.keras.layers.Dense(1))

model_4 = tf.keras.Sequential()
model_4.add(tf.keras.layers.Dense(1, input_shape=(1,)))
model_4.add(tf.keras.layers.Dense(4, activation="tanh"))
model_4.add(tf.keras.layers.Dense(4, activation="tanh"))
model_4.add(tf.keras.layers.Dense(1))

model_5 = tf.keras.Sequential()
model_5.add(tf.keras.layers.Dense(1, input_shape=(1,)))
model_5.add(tf.keras.layers.Dense(4, activation="tanh"))
model_5.add(tf.keras.layers.Dense(4, activation="tanh"))
model_5.add(tf.keras.layers.Dense(1))

model_1.compile(optimizer="adam", loss="mse")
model_2.compile(optimizer="adam", loss="mse")
model_3.compile(optimizer="adam", loss="mse")
model_4.compile(optimizer="adam", loss="mse")
model_5.compile(optimizer="adam", loss="mse")

# train model
# train model 1 on input_1_train and output_1_train
model_1.fit(input_1_train, output_1_train, epochs=100, batch_size=1000)

# train model 2 on input_2_train and output_2_train
model_2.fit(input_2_train, output_2_train, epochs=100, batch_size=1000)

# train model 3 on input_3_train and output_3_train
model_3.fit(input_3_train, output_3_train, epochs=100, batch_size=1000)

# train model 4 on input_4_train and output_4_train
model_4.fit(input_4_train, output_4_train, epochs=100, batch_size=1000)

# train model 5 on input_5_train and output_5_train
model_5.fit(input_5_train, output_5_train, epochs=100, batch_size=1000)


# %%
# predict test input with 5 models
def predict_using_5_models(input_data):
    input_data = np.array([input_data])
    if input_data < -0.5:
        return model_1.predict(input_data, verbose=0)
    elif -0.25 >= input_data > -0.5:
        return model_2.predict(input_data, verbose=0)
    elif 0.25 >= input_data > -0.25:
        return model_3.predict(input_data, verbose=0)
    elif 0.5 >= input_data > 0.25:
        return model_4.predict(input_data, verbose=0)
    elif input_data > 0.5:
        return model_5.predict(input_data, verbose=0)


lactosemodeloutput = np.array([])
for i in tqdm(range(3000)):
    # print(i, end="\r")
    lactosemodeloutput = np.append(
        lactosemodeloutput, predict_using_5_models(input_test[i])
    )

# %%
# plot general yhat
plt.plot(output_test[500:2500], "--", alpha=0.7, label="Ground Truth")
# plt.plot(generalyhat[500:2500], alpha=0.7, label="Predicted (8x8 Model)")
plt.plot(
    lactosemodeloutput[500:2500],
    "--",
    alpha=0.7,
    label="Predicted (LACTOSE 4x4 Models)",
)
plt.legend()
plt.xlabel("Samples")
plt.ylabel("Voltage(V)")
plt.title("Output Waveform Comparison")
plt.savefig("LACTOSE/figures/OutputCompareLactoseTensorflow.png")
# %%

# export models as json
from LACTOSE.lib.model_utils import *

# save_model(model, "NonLactoseModel8x8.json")
save_model(model_1, "LactoseModel1.json")  # x < -0.5
save_model(model_2, "LactoseModel2.json")  # -0.5 <= x < -0.25
save_model(model_3, "LactoseModel3.json")  # -0.25 <= x < 0.25
save_model(model_4, "LactoseModel4.json")  # 0.25 <= x < 0.5
save_model(model_5, "LactoseModel5.json")  # x >= 0.5

# save models ash5
# model.save("NonLactoseModel8x8.h5")
model_1.save("LactoseModel1.h5")
model_2.save("LactoseModel2.h5")
model_3.save("LactoseModel3.h5")
model_4.save("LactoseModel4.h5")
model_5.save("LactoseModel5.h5")

# %%

# plot transfer function of input and output
plt.plot(input_data, output_data)
plt.title("Transfer Function")
plt.xlabel("Input")
plt.ylabel("Output")
plt.grid()
plt.axvline(x=0, c="black")
plt.axhline(y=0, c="black")

# %%
