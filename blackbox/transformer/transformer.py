import numpy as np
import pandas as pd
import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader, TensorDataset
import matplotlib.pyplot as plt

TRAIN_FLAG = 1
# read csv file with np
data = pd.read_csv("blackbox/ADC_Training_Data/extended_data_array.csv")
data = data.to_numpy()
print(data.shape)

X = np.concatenate((data[:, :2], data[:, 3:]), axis=1)
y = data[:, 2]

X_train = X[: int(0.8 * len(X))]
y_train = y[: int(0.8 * len(y))]

X_test = X[int(0.8 * len(X)) :]
y_test = y[int(0.8 * len(y)) :]

print(X_train.shape)
print(y_train.shape)
print(X_test.shape)
print(y_test.shape)

# Assuming X_train, y_train, X_test, y_test are numpy arrays that are available from the previous code snippet

# Hyperparameters
d_model = 8
nhead = 2
num_encoder_layers = 8
dim_feedforward = 8
dropout = 0.1
batch_size = 1024
learning_rate = 0.001
num_epochs = 250  # Define the number of epochs for training


# Define positional encoding
class PositionalEncoding(nn.Module):
    def __init__(self, d_model, dropout=0.1, max_len=1024):
        super(PositionalEncoding, self).__init__()
        self.dropout = nn.Dropout(p=dropout)

        position = torch.arange(max_len).unsqueeze(1)
        div_term = torch.exp(torch.arange(0, d_model, 2) * -(np.log(10000.0) / d_model))
        pe = torch.zeros(max_len, 1, d_model)
        pe[:, 0, 0::2] = torch.sin(position * div_term)
        pe[:, 0, 1::2] = torch.cos(position * div_term)
        self.register_buffer("pe", pe)

    def forward(self, x):
        x = x + self.pe[: x.size(0)]
        return self.dropout(x)


# Define the small transformer model using nn.Transformer
class SmallTransformer(nn.Module):
    def __init__(self, d_model, nhead, num_encoder_layers, dim_feedforward, dropout):
        super(SmallTransformer, self).__init__()
        self.pos_encoder = PositionalEncoding(d_model, dropout)
        self.transformer_encoder = nn.TransformerEncoder(
            nn.TransformerEncoderLayer(
                d_model, nhead, dim_feedforward, dropout, batch_first=True
            ),
            num_encoder_layers,
        )
        self.encoder = nn.Linear(10, d_model)  # Assuming input dimension is 10
        self.d_model = d_model
        self.decoder = nn.Linear(d_model, 1)  # Assuming output dimension is 1

    def forward(self, src):
        src = self.encoder(src) * np.sqrt(self.d_model)
        src = self.pos_encoder(src)
        output = self.transformer_encoder(src)
        output = self.decoder(output)
        return output


# Instantiate the model
model = SmallTransformer(d_model, nhead, num_encoder_layers, dim_feedforward, dropout)

# Loss function and optimizer
criterion = nn.MSELoss()
optimizer = optim.Adam(model.parameters(), lr=learning_rate)

# Prepare data
# Assuming each feature is a timestep in a sequence
train_data = TensorDataset(
    torch.from_numpy(X_train).float().unsqueeze(1),  # Add sequence dimension
    torch.from_numpy(y_train).float(),
)
train_loader = DataLoader(train_data, batch_size=batch_size, shuffle=False)

if TRAIN_FLAG == 1:
    # Training loop
    for epoch in range(num_epochs):
        model.train()
        total_loss = 0
        for batch, (X_batch, y_batch) in enumerate(train_loader):
            optimizer.zero_grad()
            # No need to permute since we've already unsqueezed in the DataLoader
            output = model(X_batch)
            loss = criterion(
                output.squeeze(), y_batch
            )  # May need to adjust output shape to match target
            loss.backward()
            optimizer.step()
            total_loss += loss.item()
            # number of batches is len(train_data) / batch_size
            print(
                f"Epoch {epoch} Batch {batch} / {len(train_data) / batch_size} Loss: {loss.item()}",
                end="\r",
            )
        print(f"Epoch {epoch} Loss: {total_loss / len(train_loader)}")
    # Save pth
    torch.save(model.state_dict(), "blackbox/transformer/transformer.pth")
    TRAIN_FLAG = 0
if TRAIN_FLAG == 0:
    model.load_state_dict(torch.load("blackbox/transformer/transformer.pth"))
    # Evaluate the model
    model.eval()
    test_data = TensorDataset(
        torch.from_numpy(X_test).float().unsqueeze(1), torch.from_numpy(y_test).float()
    )
    test_loader = DataLoader(test_data, batch_size=batch_size, shuffle=False)

    with torch.no_grad():
        total_loss = 0
        # Use enumerate to get the batch index
        for batch_index, (X_batch, y_batch) in enumerate(test_loader):
            # Ensure that X_batch has the correct shape for the model
            # If your model is expecting a sequence, you might need X_batch.unsqueeze(1)
            output = model(X_batch)
            loss = criterion(output.squeeze(), y_batch)
            total_loss += loss.item()
            # Use batch_index for the current batch number
            print(
                f"Test Loss: {loss.item()}, {batch_index + 1}/{len(test_loader)}",
                end="\r",
            )
        print(f"\nTest Loss: {total_loss / len(test_loader)}")

    # Function to generate predictions
    def generate_predictions(model, loader):
        model.eval()
        predictions = []
        ground_truth = []

        with torch.no_grad():
            for X_batch, y_batch in loader:
                X_batch = X_batch.permute(
                    1, 0, 2
                )  # Adjust for the expected input dimensions of the model
                output = model(X_batch)
                predictions.extend(output.view(-1).tolist())  # Flatten the output
                ground_truth.extend(y_batch.tolist())

        return predictions, ground_truth

    # Get predictions and ground truth from the test set
    predictions, ground_truth = generate_predictions(model, test_loader)

    # Plotting the first N samples for visualization
    N = 100  # Number of samples to visualize
    plt.figure(figsize=(15, 5))

    # Plot ground truth
    plt.plot(ground_truth[5000:10000], label="Ground Truth", linewidth=2)

    # Plot predicted values
    plt.plot(predictions[5000:10000], label="Predictions", linewidth=2)

    plt.title("Comparison of Predictions and Ground Truth")
    plt.xlabel("Sample Index")
    plt.ylabel("Output Value")
    plt.legend()
    plt.savefig("blackbox/transformer/transformer.png")
    plt.show()
