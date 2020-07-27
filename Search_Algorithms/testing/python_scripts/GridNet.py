import torch
import torch.nn as nn 
import torch.nn.functional as F

#Unicamente como esta ahorita funciona para un grafo de 5x5.

class NNGrid(nn.Module):
    def __init__(self):
        super().__init__()
        self.conv1 = nn.Conv2d(in_channels = 3, out_channels= 5, kernel_size= 3, padding= 1)
        self.conv2 = nn.Conv2d(in_channels = 5, out_channels= 5, kernel_size= 3, padding= 1)
        self.conv3 = nn.Conv2d(in_channels = 5, out_channels= 5, kernel_size= 2, padding= 1)
        self.fc1 = nn.Linear(180,90)
        self.fc2 = nn.Linear(90,45)
        self.fc3 = nn.Linear(45,1)
        self.drop1 = nn.Dropout()
        self.drop2 = nn.Dropout()
        
    def forward(self, x):
        #print(x.shape)
        x = F.relu(self.conv1(x))
        #print(x.shape)
        x = F.relu(self.conv2(x))
        #print(x.shape)
        x = F.relu(self.conv3(x))
        #print(x.shape)
        x = x.view(-1,180)
        #print(x.shape)
        x = self.drop1(F.relu(self.fc1(x)))
        #print(x.shape)
        x = self.drop2(F.relu(self.fc2(x)))
        #print(x.shape)
        return torch.tanh(self.fc3(x))


