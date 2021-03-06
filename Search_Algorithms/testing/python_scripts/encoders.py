import torch 
import numpy as np 
import networkx as nx

class GridEncoder:
    def __init__(self, rows, cols):
        self.rows_ = rows
        self.cols_ = cols

    
    def encode_data(self,state, action_list, player):

        def update_pieces(state,Black_arr, White_arr):
            Black_arr[state == 66] = 1
            White_arr[state == 87] = 1

        if type(state) != np.ndarray:
            state = np.array(state)

        valid_ = np.zeros(self.rows_*self.cols_)
        black_ = np.zeros(self.rows_*self.cols_)
        white_ = np.zeros(self.rows_*self.cols_)

        update_pieces(state , black_, white_)
        action_list.pop() #We delete pass action.
        valid_[action_list] = 1

        data = torch.zeros((3, self.rows_, self.cols_))

        black_ = torch.tensor(black_).view((self.rows_, self.cols_))
        white_ = torch.tensor(white_).view((self.rows_, self.cols_))
        valid_ = torch.tensor(valid_).view((self.rows_, self.cols_))

        ind_white_ = 1
        ind_black_ = 0
        # When player is 1 then first plane is white.
        if player == 1:
            ind_white_ = 0
            ind_black_ = 1
        data[ind_white_] = white_
        data[ind_black_] = black_
        data[2] = valid_

        return data
        
    def encode_label(self, reward):
        return torch.tensor([float(reward)])

    #Return a list of tensors.
    def encode_episode(self, episode):
        recordings = []
        reward = episode["black_reward"]
        current_player = 0

        for i in range(len(episode["state"])):
            data = self.encode_data(episode["state"][i], episode["valid_actions"][i], current_player)
            target = self.encode_label(reward)
            recordings += [[data,target]]
            reward *= -1
            current_player^= 1

        return recordings

class GraphEncoder:
    def __init__(self, num_nodes, num_features):
        self.num_nodes = num_nodes
        self.num_node_features = num_features

    def encode_data(self, state, action_list, player):

        def update_pieces(state,Black_arr, White_arr):
            Black_arr[state == 66] = 1
            White_arr[state == 87] = 1

        if type(state) != np.ndarray:
            state = np.array(state)

        valid_ = np.zeros(self.num_nodes)
        black_ = np.zeros(self.num_nodes)
        white_ = np.zeros(self.num_nodes)

        update_pieces(state , black_, white_)
        action_list.pop() #We delete pass action.
        valid_[action_list] = 1

        ind_white_ = 1
        ind_black_ = 0
        # When player is 1 then first plane is white.
        if player == 1:
            ind_white_ = 0
            ind_black_ = 1

        data_ = np.zeros((3,self.num_nodes))
        data_[ind_white_] = white_
        data_[ind_black_] = black_
        data_[2] = valid_
        
        data_ = data_.transpose()
        return torch.tensor(data_)

    def encode_label(self, reward):
        return torch.tensor([float(reward)])

    def encode_episode(self, episode):
        recordings = []
        reward = episode["black_reward"]
        current_player = 0

        for i in range(len(episode["state"])):
            data = self.encode_data(episode["state"][i], episode["valid_actions"][i], current_player)
            target = self.encode_label(reward)
            recordings += [[data,target]]
            reward *= -1
            current_player^= 1

        return recordings