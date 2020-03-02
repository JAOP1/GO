
#pragma once
#include "../BoardGame.hpp"
#include <limits>
#include <string>
#include <vector>

using Action = int;

class MinMaxSearch
{

public:
    MinMaxSearch(int level, char current_player)
        : max_depth(level), player(current_player)
    {}

    Action search(BoardGame current_board)
    {
        auto board = current_board;
        int alpha = std::numeric_limits<int>::min();
        int beta = std::numeric_limits<int>::max();
        int max_reward = std::numeric_limits<int>::max();
        Action best_action = -1;

        for (auto v : board.get_available_sample_cells(1.0))
        {
            // board.make_action(v);
            auto reward = get_reward(board, v, 1, -1, alpha, beta);
            // board.undo_action(v);

            if (max_reward > reward)
            {
                best_action = v;
                max_reward = reward;
            }
        }

        return best_action;
    }

private:
    int get_reward(BoardGame board,
                   Action v,
                   int depth,
                   int current_player,
                   int alpha,
                   int beta)
    {
        board.make_action(v);
        if (board.is_complete() || max_depth <= depth)
            return current_player*board.reward(player);

        int reward = std::numeric_limits<int>::max(); // Ahorita reviso esto
                                                      // mejor...
        for (auto v : board.get_available_sample_cells(1.0))
        {
            // board.make_action(v);
            auto current_reward =
              get_reward(board, v, depth + 1, current_player*-1, alpha, beta);
            // board.undo_action(v);

            reward = std::min(current_reward, reward);

            auto AlphaBetaChanges =
              UpdateAlphaBeta(alpha,
                              beta,
                              current_reward,
                              (current_player == -1 ? true : false));

            alpha = AlphaBetaChanges[0];
            beta = AlphaBetaChanges[1];

            if (beta <= alpha)
                break;
        }

        return reward*-1;
    }

    std::vector<int> UpdateAlphaBeta(int alpha,
                                     int beta,
                                     int score,
                                     bool is_min_player)
    {

        if (is_min_player)
        {
            return {alpha, std::min(beta, score)};
        }
        score *= -1;
        return {std::max(alpha, score), beta};
    }

    char player;
    int max_depth;
};