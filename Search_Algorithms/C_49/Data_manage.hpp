#pragma once 
#include <torch/torch.h>
#include <iostream>
#include <vector>
#include <numeric>
#include <tuple>
#include "Include/BoardGame.hpp"
#include "Include/Extra/Graph.hpp"
#include "Include/Extra/json_manage.hpp"
#include "Search_Algorithms/C_49/Net_Class.hpp"


using element =  torch::data::Example<torch::Tensor , torch::Tensor>;

//Estructura para recolectar los datos.
struct GameDateSet : torch::data::datasets::BatchDataset<GameDateSet, std::vector<element>, std::vector<size_t> > {
  
  explicit GameDateSet(std::vector<element>& sample) : dataset(sample) {}
  
  std::vector<element> get_batch(std::vector<size_t> indices) override {
    
    std::vector< element >  batch;

    for (auto i : indices) {
          batch.push_back(dataset[i]);
    }

    return batch;
  }
  
  torch::optional<size_t> size() const override {
    return dataset.size();
  }

  std::vector<element> dataset;
};


//Mañana realizo esta función.
template<class game>
game get_episode(Network_evaluator& Model , std::string slection_mode, BoardGame BG)
{
  game episode;

  while()
  {

  }

  return episode;
}


void generate_games(std::string path, int games , Network_evaluator& Model , std::string slection_mode, BoardGame& BG )
{
  std::vector<game> episodes;
  for(int i = 0; i < games; ++i)
  {
    episodes.push_back(get_episode(Model , selection_mode ,BG));
  }

  save_games_recordings_to_json(path ,  episodes);
}


template <class encoder>
std::vector<element> get_data_games(std::string& DataPath, encoder&  Encoder_,const BoardGame& BG)
{
  std::vector<element> X;

  std::vector<game> games_played = get_json_to_game_data<game>(DataPath);

  for(game episode :  games_played)
  {
    auto encoded_states = Encoder_.Encode_episode( episode);
    X.insert(X.end() , encoded_states.begin() , encoded_states.end());
  }

  return X;
}