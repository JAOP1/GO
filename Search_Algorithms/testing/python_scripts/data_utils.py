from torch.utils.data import Dataset,DataLoader
import torch 
import torch_geometric.data as geodata
from utils import get_json_game
import networkx as nx
from encoders import GridEncoder

class DataStruct(Dataset):

    def __init__(self, data):
        self.data_ = data

    def __getitem__(self,index):
        return self.data_[index][0], self.data_[index][1]

    def __len__(self):
        return len(self.data_)


def make_dataloader(dataset,  batch_size_=4, shuffle_ = True):
    return DataLoader(dataset,batch_size = batch_size_ , shuffle = shuffle_)

def make_GeometricDataloader(dataset, batch_size_ = 4, shuffle_ = True):
    return geodata.DataLoader(dataset, batch_size = batch_size_, shuffle=shuffle_)

def LoadDataset(path, encoder, construct_torchDataset = True):
    json_data = get_json(path)
    data = []
    for i in range(json_data["Num_Games"]):
        data += encoder.encode_episode(json_data["Game "+str(i)])

    if construct_torchDataset:
        dataset = DataStruct(data)
        return dataset

    return data

def loadGeometricDataset(datapath, graphpath, encoder):
    json_graph = get_json(graphpath)
    #Getting data.
    data = LoadDataset(datapath, encoder)
    inputs, labels = [element[0] for element in data] , [element[1] for element in data]

    #Getting edges.
    edges = json_graph["edges"]
    edges2 = [[v,u] for u,v in edges]
    edges += edges2

    #geometric data.
    edge_index = torch.tensor(edges, dtype = torch.long)   
    data_list = [] 
    for i in range(len(data)):
        x = torch.tensor(inputs[i], dtype = torch.float)
        y = torch.tensor(labels[i], dtype = torch.float)
        data_list.append(geodata.Data(x = x , edge_index = edge_index.t().contiguous() , y = y))
    return data_list
