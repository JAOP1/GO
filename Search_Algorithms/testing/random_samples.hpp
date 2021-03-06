#pragma once 
#include <string>
#include <vector>
#include "../Standard_Engines/random_agent.hpp"
#include "../../Include/Extra/json_manage.hpp"
#include "../../Include/Extra/Graph.hpp"
#include "../../Include/BoardGame.hpp"
#include "../C_49/Grid/Include/Data_manage.hpp"


/*
    para ejecutar en consola este test puede ser así:
    ./tester -i grid5.json -n 0
*/

void generate_random_samples(int num_games,Graph G, std::string data_path)
{
    BoardGame BG(G);
    Random_agent Black(G.num_vertices(), 'B');
    Random_agent White(G.num_vertices(), 'W');

    game_utils::generate_games<Random_agent>(data_path , num_games, BG, Black, White, 0.7);
    
}