from torch.utils.data import Dataset,DataLoader
from utils import get_json_game
from GridEncoder import GridEncoder

class DataGames(Dataset):

    def __init__(self, data):
        self.data_ = data

    def __getitem__(self,index):
        return self.data_[index]

    def __len__(self):
        return len(self.data_)


def make_dataloader(dataset,  batch_size_=4, shuffle_ = True):
    return DataLoader(dataset,batch_size = batch_size_ , shuffle = shuffle_)

#It works for our purpose.
#Ojo aqui!!! Solo funcionará para nuestra grafica de 5x5.
def LoadDataset(path):
    json_data = get_json_game(path)
    encoder_ = GridEncoder(5,5)
    data = []
    for i in range(json_data["Num_Games"]):
        data += [encoder_.encode_episode(json_data["Game "+str(i)])]
    
    return make_dataloader(data)
