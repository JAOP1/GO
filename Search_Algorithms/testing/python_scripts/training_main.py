#User imports.
from  encoders import GridEncoder, GraphEncoder
from data_utils import *
from GridNet import NNGrid
import GraphNet as models
from utils import *
#Community imports.
import torch
import torch.optim as optim
import torch.nn as nn 
import os

def train_net(net, trainloader, num_epoch, criterion, optimizer, device,Path):
    """
    Net: Pytorch geometric model.
    Trainloader:  Data Manager (it is in pytorch)
    num_epoch: the times to operate all dataset.
    criterion: The loss function.
    optimizer: Adam, SGD, etc.
    device: CPU or GPU.
    Path: where allocate network results.
    """
    global data_size
    net.to(device)

    for epoch in range(num_epoch):  # loop over the dataset multiple times

        #running_loss = 0.0
        total_loss=0.0
        for i, data in enumerate(trainloader, 0):
            #dataloader de torch geometric.
            data = data.to(device)

            #Esto funciona cuando hablamos de un dataloader normal
            # get the inputs; data is a list of [inputs, labels]
            #inputs = data[0].to(device)
            #labels = data[1].to(device)

            # zero the parameter gradients
            optimizer.zero_grad()

            # forward + backward + optimize
            outputs = net(data)
            #Dataloader torch geometric
            loss = criterion(outputs, data.y)

            #Dataloader torch.
            #loss = criterion(outputs, labels)
            loss.backward()
            optimizer.step()

            # print statistics
            total_loss+=loss.item()
        
        print('Average loss by epoch = {}'.format(total_loss/data_size))
        
    if Path != "":
        torch.save(RewardNet.state_dict(), Path)    

@torch.no_grad()
def test(net, testloader):
    net.eval()
    total_correct = 0
    for data in testloader:
        data = data.to(device)
        out = net(data)
        pred = out.max(dim=1)[1]
        total_correct += pred.eq(data.y).sum().item()
    return total_correct / len(testloader.dataset)


if __name__ == "__main__":

    # Assuming that we are on a CUDA machine, this should print a CUDA device:
    device_type = "cpu"
    if torch.cuda.is_available():
        print("cuda disponible")
        device_type = "cuda:0"

    device = torch.device(device_type)
    print(device)
    #Data path
    Current_dir = os.path.abspath(os.getcwd())
    Data_path = Current_dir+"/../Data_grid5.json" 
    Graph_path = Current_dir+"/../../../Graphs/grid5.json"
    #Load dataset.
    print("-----------------")
    print("Loading dataset.")
    #-------------------------------------
    #Esto funciona unicamente en una grid de 5 x 5 (ajustar los valores de GridEncoder para otro tipo).
    #encoder_ = GridEncoder(5,5)
    #data = LoadDataset(Data_path, encoder_)
    #DataLoader = make_dataloader(data, 110)
    #-------------------------------------
    #Esto funciona pa todo grafo (se supone Gg)
    #Num nodos: 25, Num features: 3
    encoder_ = GraphEncoder(25,3) 
    data = loadGeometricDataset(Data_path, Graph_path, encoder_)
    DataLoader = make_GeometricDataloader(data, 110)
    #-------------------------------------

    data_size = len(data)
    #Create grid net.
    print("-----------------")
    print("Initializing network.")
    #Esta red es para un grafo en malla.
    #RewardNet = NNGrid()
    #Esta red usa capas Gin.
    RewardNet =models.NNGinConv(3, 1)
    #Esta red usa capas GCN.
    #RewardNet = models.NNGcnConv(3, 1)
    RewardNet.train()
    #Loss function and optimizer.
    print("-----------------")
    print("Initializing optimizer and loss measure.")
    criterion_ = nn.MSELoss()
    optimizer_ = optim.Adam(RewardNet.parameters())
    #Train Model and save.
    print("-----------------")
    print("Starting to train.")
    PATH = Current_dir + "/GridNet5.pth"
    train_net(RewardNet, DataLoader, 10, criterion_, optimizer_, device, PATH)

