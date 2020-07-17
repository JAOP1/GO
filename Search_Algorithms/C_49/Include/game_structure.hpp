#pragma once
#include <vector>

using vertex = int;

struct game
{
    explicit game() {}
    explicit game(
                //std::vector<std::vector<double>> prob,
                  std::vector<std::vector<vertex>> valid_actions_,
                  std::vector<std::vector<char>> states_,
                  int black_reward_)

        : states(states_)
        //, probabilities(prob)
        , valid_actions(valid_actions_)
        , black_reward(black_reward_)
    {}

    int episode_size() const { return states.size(); }

    int get_reward() const { return black_reward; }

    void set_reward(int value) { black_reward = value; }

    void add_game_state(
                        //const std::vector<double>& prob,
                        const std::vector<char>& state,
                        const std::vector<vertex>& valid_actions_)
    {
        states.push_back(state);
        //probabilities.push_back(prob);
        valid_actions.push_back(valid_actions_);
    }

    std::vector<std::vector<char>> states;
    //std::vector<std::vector<double>> probabilities;
    std::vector<std::vector<vertex>> valid_actions;
    int black_reward;
};
