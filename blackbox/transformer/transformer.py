# %%
import torch
import torch.nn as nn
from torch.utils.data import TensorDataset, DataLoader
import torchaudio
import os


# Function to load audio file
def load_audio_file(file_path, channel=0):
    waveform, sample_rate = torchaudio.load(file_path)
    waveform = waveform[channel]
    return waveform


# Function to extract resistance from file name
def extract_resistance(file_name):
    resistance_str = file_name.split("recorded_")[1].split("k.wav")[0]
    try:
        resistance = float(resistance_str) * 1000
        return resistance
    except ValueError:
        return None


# Define the Transformer Model
class AudioTransformer(nn.Module):
    def __init__(
        self, d_model, nhead, num_encoder_layers, num_decoder_layers, resistance_dim=1
    ):
        super(AudioTransformer, self).__init__()
        self.embedding = nn.Linear(1 + resistance_dim, d_model)
        self.transformer = nn.Transformer(
            d_model, nhead, num_encoder_layers, num_decoder_layers
        )
        self.fc = nn.Linear(d_model, 1)

    def forward(self, src, resistance):
        # Ensure src and resistance have the same dimensions except in dim=2
        src = src.unsqueeze(2)
        resistance = resistance.expand(-1, -1, src.size(2))
        concatenated = torch.cat((src, resistance), dim=2)
        embedded = self.embedding(concatenated)
        output = self.transformer(embedded, embedded)
        return self.fc(output)


# %%
# Loading data
input_audio = "input/INPUTFILE.wav"
input_tensor = load_audio_file(input_audio)
output_tensors = []
resistances = []

for file in os.listdir("output"):
    if file.endswith(".wav"):
        file_path = os.path.join("output", file)
        output_tensors.append(load_audio_file(file_path))
        resistance = extract_resistance(file)
        resistances.append(resistance)

# Stacking output tensors and preparing resistance tensor
stacked_output_tensor = torch.stack(output_tensors)
resistance_tensor = torch.tensor(resistances).unsqueeze(-1).unsqueeze(-1)
resistance_tensor = resistance_tensor.expand(-1, input_tensor.size(0), -1)
input_tensor_expanded = input_tensor.unsqueeze(0).expand(
    stacked_output_tensor.size(0), -1, -1
)
print(f"input_tensor shape: {input_tensor.size()}")
print(f"stacked_output_tensor shape: {stacked_output_tensor.size()}")
print(
    f"resistance_tensor shape before expand: {torch.tensor(resistances).unsqueeze(1).unsqueeze(0).size()}"
)
print(f"resistance_tensor shape after expand: {resistance_tensor.size()}")
print(f"input_tensor_expanded shape: {input_tensor_expanded.size()}")
# %%
# Verifying tensor dimensions
assert (
    input_tensor.size(0) == stacked_output_tensor.size(1) == resistance_tensor.size(1)
), "Size mismatch between tensors"

# Creating dataset and dataloader
dataset = TensorDataset(input_tensor_expanded, stacked_output_tensor, resistance_tensor)

# ... rest of your code ...
batch_size = 32
shuffle = True
num_workers = 0
data_loader = DataLoader(
    dataset, batch_size=batch_size, shuffle=shuffle, num_workers=num_workers
)

# Initializing model, loss function, and optimizer
model = AudioTransformer(
    d_model=32,
    nhead=8,
    num_encoder_layers=2,
    num_decoder_layers=2,
    resistance_dim=1,
)
criterion = nn.MSELoss()
optimizer = torch.optim.Adam(model.parameters(), lr=0.001)

# Training loop
epochs = 10
for epoch in range(epochs):
    for batch in data_loader:
        inputs, targets, resistances = batch
        outputs = model(inputs, resistances)
        loss = criterion(outputs, targets)

        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

        # Optionally, log loss value here
