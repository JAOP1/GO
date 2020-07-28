#User imports.
from  encoders import GridEncoder
from data_utils import LoadDataset, make_dataloader
from GridNet import NNGrid
#Community imports.
import torch
import torch.optim as optim
import torch.nn as nn 
import os

def train_net(net, trainloader, num_epoch, criterion, optimizer, device,Path):
    global data_size
    net.to(device)

    for epoch in range(num_epoch):  # loop over the dataset multiple times

        #running_loss = 0.0
        total_loss=0.0
        for i, data in enumerate(trainloader, 0):
            # get the inputs; data is a list of [inputs, labels]
            inputs = data[0].to(device)
            labels = data[1].to(device)

            # zero the parameter gradients
            optimizer.zero_grad()

            # forward + backward + optimize
            outputs = net(inputs)
 
            loss = criterion(outputs, labels)

            loss.backward()
            optimizer.step()

            # print statistics
            total_loss+=loss.item()

        print('Average loss by epoch = {}'.format(total_loss/data_size))
        
    if Path != "":
        torch.save(RewardNet.state_dict(), Path)    

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
    #Load dataset.
    print("-----------------")
    print("Loading dataset.")
    encoder_ = GridEncoder(5,5)
    data = LoadDataset(Data_path, encoder_)
    data_size = len(data)
    DataLoader = make_dataloader(data, 110)
    #Create grid net.
    print("-----------------")
    print("Initializing network.")
    RewardNet = NNGrid()
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

