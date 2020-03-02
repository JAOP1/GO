#pragma once

#include "NumberTheory.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>
#include <vector>

// Regresa num de nodos por fila y su distancia
std::vector<int> EdgeSizeNodes(int num_nodes, int widht, int height)
{
    double nodes_by_row = std::sqrt(num_nodes);

    if (fmod(nodes_by_row, 1.0))
    {
        nodes_by_row = floor(nodes_by_row);
        nodes_by_row++;
    }

    std::vector<int> arreglo = {(int)nodes_by_row,
                                (int)ceil(widht/nodes_by_row)};
    return arreglo;
}

// Utilizamos esta funcion en GUI/Graph con el objetivo de saber si hay
// colisiones.
double distance(const sf::Vector2i& A, const sf::Vector2i& B)
{
    double X = std::pow(A.x - B.x, 2);
    double Y = std::pow(A.y - B.y, 2);

    return std::sqrt(X + Y);
}

/*
using board_state = std::vector<int>

struct Result_Game
{
    Result_Game(std::vector<>):{}
};


template< class E, class T>
double evaluate(E& Agent1, T& Agent2 )
{
    double Agent1_won = 0;
    double Agent2_won = 0;

    for(int round  = 0 ;  round < num_rounds; ++round)
    {
        reward = simulated_game(Agent1, Agent2 , false); //First engine, Second
engine and save history.
    }


    for round in range(Parameters.ROUNDS):
        game, reward = combat_game(Agent1, Agent2, save_history=False)

        if reward > 0.4:
            Agent1_won += 1

        elif reward <0.4:
            Agent2_won +=1

    return Agent1_won/(Agent1_won + Agent2_won)
}

*/