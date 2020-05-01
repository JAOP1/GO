#pragma once 
#include <torch/torch.h>
#include <iostream>
#include <vector>
#include <numeric>
#include "Include/BoardGame.hpp"
#include "Search_Algorithms/C_49/Net_Class.hpp"


using element =  torch::data::Example<torch::Tensor , torch::Tensor>;

struct GameDateset : torch::data::datasets::BatchDataset< GameDateSet, std::vector<element>, std::vector<size_t> > {
  
  explicit GameDateset(std::vector<torch::Tensor>& X , std::vector<int>& Y) : data(X) , target(Y) {}
  
  std::vector<element> get_batch(std::vector<size_t> indices) override {
    
    std::vector< element >  batch;

    for (auto i : indices) {
          batch.emplace_back(data[i] ,  target[i]);
    }

    return batch;
  }
  
  torch::optional<size_t> size() const override {
    return data.size();
  }

  std::vector<torch::Tensor> data;
  std::vector<torch::Tensor> target;
};

void generate_game(std::string path, int games , Network_evaluator& Model ,BoardGame G )
{


}

template <class encoder>
GameDataset get_data_games(std::string& DataPath, encoder&  Encoder_)
{
  std::vector<torch::Tensor> X;
  std::vector<torch::Tensor> Y;

  SimpleEncoder 

  return GameDateset(X,Y).map(torch::data::transforms::Stack<>());
}