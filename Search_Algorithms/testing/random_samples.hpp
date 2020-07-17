#pragma once 
#include <string>
#include <vector>
#include "../C_49/Include/game_structure.hpp"
#include "../Standard_Engines/random_agent.hpp"
#include "../../Include/Extra/json_manage.hpp"
#include "../../Include/Extra/Graph.hpp"
#include "../../Include/BoardGame.hpp"
#include "../C_49/Include/Data_manage.hpp"

void generate_random_samples(int num_games,Graph G, std::string data_path)
{
    BoardGame BG(G);
    std::vector<int> pseudo_encoder(G.num_vertices() , 0);
    std::vector<double> pseudo_nn(1, -1);
    //Esto está horrible, pero es para no re implementar de nuevo... :c 
    generate_games<Random_agent, std::vector<int>, std::vector<double>>(data_path , num_games, pseudo_nn, BG, pseudo_encoder);
    
}