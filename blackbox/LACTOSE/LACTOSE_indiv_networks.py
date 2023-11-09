import torch
import torch.nn as nn
import torch.optim as optim
import torchaudio
import torch.nn.init as init
from torch.utils.data import Dataset, DataLoader
from tqdm import tqdm
import os
import numpy as np

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
# Set path to ADC_VAModelling_2023
# os.chdir("../..")
# print(os.getcwd())  # turn off if running in terminal, only for jupyter interactive
# list files in dir
print(os.listdir())
inputfile_path = "blackbox/ADC_Training_Data/train_input_96k.wav"
outputfile_path = "blackbox/ADC_Training_Data/out_10.wav"

N = 8
batch_size = 128  # You can adjust the batch size here


def create_recurrent_features(waveform, N):
    # Ensure waveform is a numpy array and flatten in case it's not 1D
    waveform = np.ravel(waveform)

    # Padding the waveform at the beginning with zeros
    padded_waveform = np.pad(waveform, (N, 0), "constant", constant_values=0)

    # Create the recurrent feature set
    recurrent_features = np.array(
        [padded_waveform[t - N : t + 1] for t in range(N, len(padded_waveform))]
    )

    return recurrent_features


def separate_into_ranges(input_waveform_np, target_waveform_np, boundaries, N):
    # Ensure that the input and target are flat numpy arrays
    input_waveform_np = np.ravel(input_waveform_np)
    target_waveform_np = np.ravel(target_waveform_np)

    # Create recurrent features for the input
    input_recurrent_features = create_recurrent_features(input_waveform_np, N)

    # Slice the target waveform to match the length of the recurrent features
    sliced_target_waveform_np = target_waveform_np[N:]

    # Initialize lists to hold the categorized datasets
    datasets_input = [np.array([]) for _ in range(len(boundaries) - 1)]
    datasets_target = [np.array([]) for _ in range(len(boundaries) - 1)]

    # Iterate over each boundary range and filter the arrays
    for i in range(len(boundaries) - 1):
        # Use the condition on the last sample of input_recurrent_features
        condition = (input_recurrent_features[:, -1] >= boundaries[i]) & (
            input_recurrent_features[:, -1] < boundaries[i + 1]
        )

        # Use the condition to filter the input features
        datasets_input[i] = input_recurrent_features[condition]

        # Make sure the condition is correctly applied to the target waveform
        # Use the condition to get the corresponding target values
        # The condition's length must match the length of sliced_target_waveform_np
        datasets_target[i] = sliced_target_waveform_np[
            condition[: len(sliced_target_waveform_np)]
        ]

    return datasets_input, datasets_target


def weights_init_xavier(m):
    classname = m.__class__.__name__
    if classname.find("Linear") != -1:
        init.xavier_uniform_(m.weight)
        if m.bias is not None:
            init.constant_(m.bias.data, 0)


# Define the neural network
class SimpleAudioNet(nn.Module):
    def __init__(self, N):
        super(SimpleAudioNet, self).__init__()
        # Main branch for audio features
        self.fc1_audio = nn.Linear(N + 1, 24)
        self.fc2_audio = nn.Linear(24, 24)

        # Parameter branch for the additional input parameter
        self.fc1_param = nn.Linear(
            1, 8
        )  # A smaller branch for the single scalar parameter
        self.fc2_param = nn.Linear(8, 8)

        # Combined layer after concatenating the outputs from both branches
        self.fc_combined = nn.Linear(
            24 + 8, 24
        )  # Size 24 from audio + 8 from parameter
        self.fc3 = nn.Linear(24, 1)

    def forward(self, audio_input, param_input):
        # Main audio branch
        audio_output = torch.relu(self.fc1_audio(audio_input))
        audio_output = torch.relu(self.fc2_audio(audio_output))

        # Parameter branch
        param_output = torch.relu(self.fc1_param(param_input))
        param_output = torch.relu(self.fc2_param(param_output))

        # Combine the outputs from both branches
        combined_output = torch.cat((audio_output, param_output), dim=1)

        # Final layers after combination
        combined_output = torch.relu(self.fc_combined(combined_output))
        output = self.fc3(combined_output)
        return output


# Custom Dataset class
class WaveformDataset(Dataset):
    def __init__(self, input_waveform, target_waveform):
        self.input_waveform = input_waveform
        self.target_waveform = target_waveform

    def __len__(self):
        # Ensure the length accounts for the history size N
        return len(self.input_waveform) - N

    def __getitem__(self, idx):
        input_sample = self.input_waveform[idx]
        target_sample = self.target_waveform[idx]
        return input_sample, target_sample


# Load your .wav files
# Example usage:
input_waveform, input_sample_rate = torchaudio.load(inputfile_path)
target_waveform, target_sample_rate = torchaudio.load(outputfile_path)
input_waveform_np = input_waveform.squeeze().numpy()
target_waveform_np = target_waveform.squeeze().numpy()
input_waveform_np = input_waveform_np[N:]  # Trim the input array to valid indices
target_waveform_np = target_waveform_np[N:]  # Trim the target array to valid indices

# Check if the sample rates are equal
assert input_sample_rate == target_sample_rate, "Sample rates do not match."

boundaries = np.array(
    [np.min(input_waveform_np), -0.7, -0.3, 0.0, 0.3, 0.7, np.max(input_waveform_np)]
)
input_datasets, target_datasets = separate_into_ranges(
    input_waveform_np, target_waveform_np, boundaries, N=N
)
# Instantiate the neural network
net = SimpleAudioNet(N=N)
net.apply(weights_init_xavier)
# Define a loss function and optimizer
criterion = nn.MSELoss()
optimizer = optim.Adam(net.parameters(), lr=0.001)
# scheduler = torch.optim.lr_scheduler.StepLR(optimizer, step_size=1, gamma=0.1)
for i in range(len(input_datasets)):
    print(f"Training network with condition {i}/{len(input_datasets)}")
    input_0 = input_datasets[i]  # This should already have the shape [num_samples, N]
    target_0 = target_datasets[
        i
    ]  # This should have the shape [num_samples, 1] or just [num_samples]

    # Convert numpy arrays to tensors
    input_waveform_tensor = torch.from_numpy(input_0).float()
    target_waveform_tensor = torch.from_numpy(target_0).float()

    # Ensure that input_waveform_tensor is of shape (num_samples, N)
    # Since input_waveform_tensor is already 2D, we don't need to unsqueeze it
    # Similarly, target_waveform_tensor should be either (num_samples, 1) or just (num_samples)

    # Create dataset and dataloader
    waveform_dataset = WaveformDataset(input_waveform_tensor, target_waveform_tensor)

    waveform_dataloader = DataLoader(
        waveform_dataset, batch_size=batch_size, shuffle=False, drop_last=True
    )

    # Training loop
    # Training loop
    num_epochs = 50
    print("Starting Training...")
    for epoch in range(num_epochs):
        total_loss = 0
        with tqdm(waveform_dataloader, unit="batch") as tepoch:
            for input_batch, target_batch in tepoch:
                tepoch.set_description(f"Epoch {epoch+1}/{num_epochs}")

                # Create a tensor for the parameter input, assuming it's a scalar value of 1.0
                # The size of the parameter tensor should match the batch size
                param_input_batch = torch.full(
                    (input_batch.size(0), 1), 1.0, dtype=torch.float32
                )

                # Transfer tensors to the same device as the model
                input_batch = input_batch.to(device)
                param_input_batch = param_input_batch.to(device)
                target_batch = target_batch.to(device)

                # zero the parameter gradients
                optimizer.zero_grad()

                # forward pass - include both input_batch and param_input_batch
                outputs = net(input_batch, param_input_batch)

                # Reshape the target to match the output shape if necessary
                if target_batch.ndim == 1:
                    target_batch = target_batch.unsqueeze(-1)

                # compute the loss
                loss = criterion(outputs, target_batch)

                # backward pass to compute the gradient of loss w.r.t. network parameters
                loss.backward()

                # update the weights
                optimizer.step()

                # accumulate loss
                batch_loss = loss.item() * input_batch.size(0)
                total_loss += batch_loss
                tepoch.set_postfix(loss=batch_loss)

        average_loss = total_loss / len(waveform_dataset)
        print(
            f"Epoch [{epoch+1}/{num_epochs}] completed: Avg. Loss: {average_loss:.6f}"
        )

    print("Finished Training")

    # Optionally save the model
    torch.save(net.state_dict(), f"model_condition_{i}.pth")

    # Reinstantiate the neural network with the same history size (N)
    net = SimpleAudioNet(N=N)
    net.apply(weights_init_xavier)
    net.to(device)  # Remember to move the new model to the device
