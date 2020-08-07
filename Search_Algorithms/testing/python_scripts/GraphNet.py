import torch
import torch.nn as nn 
import torch.nn.functional as F
from torch.nn import Sequential, Linear, ReLU
from torch_geometric.nn import GINConv, global_add_pool, GCNConv



class NNGinConv(torch.nn.Module):
    def __init__(self, node_features, classes):
        super(NNGinConv, self).__init__()


        nn1 = Sequential(Linear(node_features, 32), ReLU(), Linear(32, 64), ReLU() , Linear(64,128))
        self.conv1 = GINConv(nn1)
        self.bn1 = nn.BatchNorm1d(128)

        nn2 = Sequential(Linear(128, 128), ReLU(), Linear(128, 64), ReLU() , Linear(64,32))
        self.conv2 = GINConv(nn2)
        self.bn2 = nn.BatchNorm1d(32)

        nn3 = Sequential(Linear(32, 32), ReLU(), Linear(32, 16))
        self.conv3 = GINConv(nn3)
        self.bn3 = nn.BatchNorm1d(16)

        self.fc1 = Linear(16, 16)
        self.fc2 = Linear(16, classes)

    def forward(self,data):
        x, edge_index, batch = data.x, data.edge_index, data.batch

        x = F.relu(self.conv1(x, edge_index))
        x = self.bn1(x)
        x = F.relu(self.conv2(x, edge_index))
        x = self.bn2(x)
        x = F.relu(self.conv3(x, edge_index))
        x = self.bn3(x)
        
        #x = global_add_pool(x, batch)
        x = F.relu(self.fc1(x))
        x = F.dropout(x, p=0.5, training=self.training)
        x = self.fc2(x)
        return torch.tanh(x)

class NNGcnConv(torch.nn.Module):
    def __init__(self, node_features, classes):
        super(NNGcnConv, self).__init__()
        self.conv1 = GCNConv(node_features, 16)
        self.conv2 = GCNConv(16, 32)
        self.conv3 = GCNConv(32,64)
        self.conv4 = GCNConv(64,128)
        self.fc1 = Linear(128, 32)
        self.fc2 = Linear(32, classes)

    def forward(self,data):
        x, edge_index = data.x, data.edge_index
        x = F.relu(self.conv1(x, edge_index))
        #x = F.dropout(x, training=self.training)
        x = F.relu(self.conv2(x, edge_index))
        x = F.relu(self.conv3(x, edge_index))
        x = F.relu(self.conv4(x, edge_index))

        x =  F.relu(self.fc1(x))
        x = F.dropout(x, p=0.5, training=self.training)
        x = self.fc2(x)

        return F.tanh(x)
