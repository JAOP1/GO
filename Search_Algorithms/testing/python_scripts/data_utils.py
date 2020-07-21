from torch.utils.data import Dataset,DataLoader
from utils import get_json_game
from encoders import GridEncoder

class DataStruct(Dataset):

    def __init__(self, data):
        self.data_ = data

    def __getitem__(self,index):
        return self.data_[index]

    def __len__(self):
        return len(self.data_)


def make_dataloader(dataset,  batch_size_=4, shuffle_ = True):
    return DataLoader(dataset,batch_size = batch_size_ , shuffle = shuffle_)

def LoadDataset(path, encoder):
    json_data = get_json_game(path)
    data = []
    for i in range(json_data["Num_Games"]):
        data += encoder.encode_episode(json_data["Game "+str(i)])
    print(data[:3])
    dataset = DataStruct(data)

    return dataset
