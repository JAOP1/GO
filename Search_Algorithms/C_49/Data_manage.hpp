#pragma once
#include "Include/BoardGame.hpp"
#include "Include/Extra/Graph.hpp"
#include "Include/Extra/json_manage.hpp"
#include "Search_Algorithms/C_49/Net_Class.hpp"
#include <iostream>
#include <numeric>
#include <torch/torch.h>
#include <tuple>
#include <vector>

using element = torch::data::Example<torch::Tensor, torch::Tensor>;

// Estructura para recolectar los datos.
struct GameDateSet
    : torch::data::datasets::
        BatchDataset<GameDateSet, std::vector<element>, std::vector<size_t>>
{

    explicit GameDateSet(std::vector<element>& sample) : dataset(sample) {}

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

// Solo le falta incluir los parametros de los algoritmos de busqueda.
template <class game>
game get_episode(Network_evaluator& Model, std::string slection_mode, BoardGame BG)
{
    game episode;
    std::vector<char> state;
    std::vector<double> prob;
    std::vector<int> valid_actions;
    MCT_Net Black(); // Esto lo incluiré cuando haya hecho MCT_Net
    MCT_Net White(); //...

    for (int move = 0; move < 70 && !BG.is_complete(); ++move)
    {
        state = BG.show_current_state();

        if (move%2)
        {
            auto action = White.search(BG);
            prob = White.get_probabilities_current_state();
        }
        else
        {
            auto action = Black.search(BG);
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

void generate_games(std::string path,
                    int games,
                    Network_evaluator& Model,
                    std::string slection_mode,
                    BoardGame& BG)
{
    std::vector<game> episodes;
    for (int i = 0; i < games; ++i)
    {
        episodes.push_back(get_episode(Model, selection_mode, BG));
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