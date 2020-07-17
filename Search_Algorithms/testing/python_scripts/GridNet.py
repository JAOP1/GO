import torch.nn as nn 
import torch.nn.functional as F

#Unicamente como esta ahorita funciona para un grafo de 5x5.

class NNGrid(nn.Module):
    def __init__(self):
        super().__init__()
        self.conv1 = nn.Conv2d(3,5,3)
        self.conv2 = nn.Conv2d(5,5,3)
        self.conv3 = nn.Conv2d(5,5,2)
        self.fc1 = nn.Linear(180,90)
        self.fc2 = nn.Linear(90,45)
        self.fc3 = nn.Linear(45,1)
        self.drop1 = nn.Dropout()
        self.drop2 = nn.Dropout()
        
    def forward(self, x):
        x = F.relu(self.conv1(x))
        x = F.relu(self.conv2(x))
        x = F.relu(self.conv3(x))

        x = x.view({180,-1})
        x = self.drop1(F.relu(self.fc1(x)))
        x = self.drop2(F.relu(self.fc2(x)))
        return F.tanh(self.fc3(x))


