#pragma once 
#include <iostream>
#include <torch/torch.h>
#include <vector>
#include <numeric>

using element =  torch::data::Example<torch::Tensor , std::vector<double>>;

struct GameDateSet : torch::data::datasets::BatchDataset< GameDateSet, std::vector<element>, std::vector<size_t> > {
  
  explicit GameDateSet(std::vector<torch::Tensor>& X , std::vector<int>& Y) : data(X) , target(Y) {}
  
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
  std::vector<std::vector<double>> target;
};
