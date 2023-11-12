import torch
import torch.nn as nn
from torchviz import make_dot

# Define the simplified model with abstracted components
class AbstractedTransformer(nn.Module):
    def __init__(self):
        super(AbstractedTransformer, self).__init__()
        self.abstracted_encoder = nn.Linear(10, 1)  # Abstracted encoder block
        self.abstracted_transformer = nn.Linear(1, 1)  # Abstracted transformer block

    def forward(self, src):
        src = self.abstracted_encoder(src)
        output = self.abstracted_transformer(src)
        return output
        
# Instantiate the abstracted model
abstracted_model = AbstractedTransformer()

# Create a dummy input tensor
dummy_input = torch.randn(1, 10)

# Perform a forward pass to get the output
output = abstracted_model(dummy_input)

# Generate the abstracted visual graph
visual_graph = make_dot(output, params=dict(list(abstracted_model.named_parameters())))

# Visualize the abstracted graph
visual_graph.view()
