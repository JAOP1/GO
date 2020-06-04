#pragma once
#include "../../Include/BoardGame.hpp"
#include "../../Include/Extra/Graph.hpp"
#include "../../Include/Extra/json_manage.hpp"
#include "game_structure.hpp"
#include "Net_Class.hpp"
#include "encoders.hpp"
#include <torch/torch.h>
#include <iostream>
#include <numeric>
#include <tuple>
#include <vector>

using element = torch::data::Example<torch::Tensor, torch::Tensor>;

// Estructura para recolectar los datos.
struct GameDataSet: torch::data::datasets::BatchDataset<GameDataSet, std::vector<element>, std::vector<size_t>>
{

    explicit GameDataSet(std::vector<element>& sample) : dataset(sample) {}

    std::vector<element> get_batch(std::vector<size_t> indices) override
    {

        std::vector<element> batch;

        for (auto i : indices)
        {
            batch.push_back(dataset[i]);
        }

        return batch;
    }

    torch::optional<size_t> size() const override { return dataset.size(); }

    std::vector<element> dataset;
};

template<class search_type, class Encoder>
game get_episode(Network_evaluator& Model, BoardGame BG, Encoder& encoder_)
{
    game episode;
    std::vector<char> state;
    std::vector<double> prob;
    std::vector<int> valid_actions;
    int action;

    search_type Black(BG , Model , encoder_ , 20, 'B'); 
    search_type White(BG, Model, encoder_ , 20 , 'W'); 

    for (int move = 0; move < 70 && !BG.is_complete(); ++move)
    {
        state = BG.show_current_state();

        if (move%2)
        {
            action = White.search(BG);
            prob = White.get_probabilities_current_state();
        }
        else
        {
            action = Black.search(BG);
            prob = Black.get_probabilities_current_state();
        }
        Black.fit_precompute_tree(action);
        White.fit_precompute_tree(action);

        valid_actions = BG.get_available_sample_cells(1.0);
        BG.make_action(action);

        episode.add_game_state(prob, state, valid_actions);
    }

    episode.set_reward(BG.reward('B'));

    return episode;
}

template<class search_type,class Encoder>
void generate_games(std::string path,
                    int games,
                    Network_evaluator& Model,
                    BoardGame& BG,
                    Encoder& encoder_
                    )
{
    std::vector<game> episodes;
    for (int i = 0; i < games; ++i)
    {
        episodes.push_back(get_episode<search_type,Encoder>(Model, BG, encoder_));
    }

    save_games_recordings_to_json(path, episodes);
}

template <class encoder>
std::vector<element> get_data_games(std::string& DataPath, encoder& Encoder_)
{
    std::vector<element> X;

    std::vector<game> games_played = get_json_to_game_data<game>(DataPath);

    for (game episode : games_played)
    {
        auto encoded_states = Encoder_.Encode_episode(episode);
        X.insert(X.end(), encoded_states.begin(), encoded_states.end());
    }

    return X;
}