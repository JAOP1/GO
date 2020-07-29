#pragma once
#include "../../../Include/BoardGame.hpp"
#include "../../../Include/Extra/Graph.hpp"
#include "../../../Include/Extra/json_manage.hpp"
#include "../Encoders_classes/encoders.hpp"
#include "../../../Include/Extra/printable.hpp"
#include "game_structure.hpp"
#include <torch/torch.h>
#include <iostream>
#include <numeric>
#include <tuple>
#include <vector>

using element = torch::data::Example<torch::Tensor, torch::Tensor>;



// Estructura para recolectar los datos.
struct GameDataSet
    : torch::data::datasets::
        BatchDataset<GameDataSet, std::vector<element>, std::vector<size_t>>
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

namespace game_utils
{
    template <class search_type, class Encoder, class NN>
    game get_episode(NN& Model, BoardGame BG, Encoder& encoder_)
    {
        /*
        Description:
            Return a game episode using the current model
            and the search method.
        */
        game episode;
        std::vector<char> state;
        //std::vector<double> prob;
        std::vector<int> valid_actions;
        int action;

        search_type Black(BG, Model, encoder_, 100, 'B');
        search_type White(BG, Model, encoder_, 100, 'W');

        for (int move = 0; move < 70 && !BG.is_complete(); ++move)
        {
            state = BG.show_current_state();

            if (move%2)
            {
                action = White.search(BG);
                //prob = White.get_probabilities_current_state();
            }
            else
            {
                action = Black.search(BG);
                //prob = Black.get_probabilities_current_state();
            }
            Black.fit_precompute_tree(action);
            White.fit_precompute_tree(action);
            valid_actions = BG.get_available_sample_cells(1.0);
            BG.make_action(action);

            //episode.add_game_state(prob, state, valid_actions);
            episode.add_game_state(state,valid_actions);
        }

        episode.set_reward(BG.reward('B'));

        return episode;
    }

    template< class search_type>
    game get_episode(search_type& BlackEngine, search_type& WhiteEngine, BoardGame BG)
    {
        /*
        Description:
            Return a game episode using the current model
            and the search method.
        */
        game episode;
        std::vector<char> state;
        //std::vector<double> prob;
        std::vector<int> valid_actions;
        int action;


        for (int move = 0; move < 70 && !BG.is_complete(); ++move)
        {
            state = BG.show_current_state();

            if (move%2)
            {
                action = WhiteEngine.search(BG);
                //prob = White.get_probabilities_current_state();
            }
            else
            {
                action = BlackEngine.search(BG);
                //prob = Black.get_probabilities_current_state();
            }

            BlackEngine.fit_precompute_tree(action);
            WhiteEngine.fit_precompute_tree(action);
            valid_actions = BG.get_available_sample_cells(1.0);
            BG.make_action(action);

            //episode.add_game_state(prob, state, valid_actions);
            episode.add_game_state(state,valid_actions);
        }

        episode.set_reward(BG.reward('B'));

        return episode;
    }


    template <class search_type, class Encoder, class NN>
    void generate_games(
    std::string path, int games, NN& Model, const BoardGame& BG, Encoder& encoder_)
    {
        /*
        Description:
            Create a game sample using our current model which 
            is saved in the path.
        */
        std::vector<game> episodes;
        for (int i = 0; i < games; ++i)
        {
            std::cout << "Generating game " << i << std::endl;
            episodes.push_back(
            get_episode<search_type, Encoder, NN>(Model, BG, encoder_));
            std::cout << "-------------------------------" << std::endl;
        }

        json_utils::save_games_recordings_to_json(path, episodes);
    }


     template <class search_type>
    void generate_games(std::string path, int games,  const BoardGame& BG, 
         search_type& BlackEngine,  search_type& WhiteEngine)
    {
        /*
        Description:
            Path: ubication allocate all records.
            games: total records.
            BG: the rule set.
            Engines: search algorithm.
        */

        std::vector<game> episodes;
        for (int i = 0; i < games; ++i)
        {
            std::cout << "Generating game " << i << std::endl;
            episodes.push_back(
            get_episode<search_type>(BlackEngine, WhiteEngine, BG));
            std::cout << "-------------------------------" << std::endl;
        }

        json_utils::save_games_recordings_to_json(path, episodes);
    }

    template <class encoder>
    std::vector<element> get_data_games(std::string& DataPath, encoder& Encoder_)
    {
        /*
        Description:
            Load every game recording saved in datapath.
        */
        std::vector<element> X;
        std::vector<game> games_played = json_utils::get_json_to_game_data<game>(
        DataPath);

        for (game episode : games_played)
        {
            auto encoded_states = Encoder_.Encode_episode(episode); 
            X.insert(X.end(), encoded_states.begin(), encoded_states.end());
        }

        return X;
    }
}