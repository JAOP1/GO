#pragma once
#include "../Include/BoardGame.hpp"
#include <vector>

using Action = int;

/*
Todos los algoritmos tienen la misma estructura, por esa razón 
se mantiene.
*/

class Random_agent
{
public:
    //Ignora todos los parametros... 
    explicit Random_agent(BoardGame& BG,std::vector<double> num2,std::vector<int>& total_actions_,int sim, char player ):
     total_actions(total_actions_.size()),
     player_(player)
     {}
    
    explicit Random_agent(int total_actions_, char player): total_actions(total_actions_) , player_(player) {}
    
    Action search(const BoardGame& current_board)
    {
        return current_board.random_action();
    }

    void fit_precompute_tree(Action A) {}

    void reset_tree(){}

    void set_player(char player) { player_ = player; }

    std::vector<double> get_probabilities_current_state() const
    {
    
        double prob_by_action = 1.0/total_actions;
        std::vector<double> probabilities(total_actions + 1, prob_by_action); // Por el pasar.
        return probabilities;
    }


private:
    int total_actions;
    char player_;
};