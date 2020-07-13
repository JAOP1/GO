#pragma once
#include "../Include/BoardGame.hpp"
#include <vector>

using Action = int;

class Random_agent
{
public:
    Random_agent() {}

    Action search(const BoardGame& current_board)
    {
        return current_board.random_action();
    }

    void fit_precompute_tree(Action A) {}

    void reset_tree() {}

    void set_player(char player) { player_ = player; }

private:
    char player_;
};