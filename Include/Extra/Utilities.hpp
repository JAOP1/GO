#pragma once

#include "NumberTheory.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>
#include <vector>



void split(const std::string& s, char c,
           std::vector<std::string>& v) {
   std::string::size_type i = 0;
   std::string::size_type j = s.find(c);

   while (j != std::string::npos) {
      v.push_back(s.substr(i, j-i));
      i = ++j;
      j = s.find(c, j);

      if (j == std::string::npos)
         v.push_back(s.substr(i, s.length()));
   }
}



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


template <class search_algorithm1, class search_algorithm2>
bool is_winner(search_algorithm1& player1,
               search_algorithm2& player2,
               BoardGame board)
{

    auto first_to_play = board.player_status();
    auto second_to_play = (first_to_play == 'B' ? 'W' : 'B');

    // Para que no realicen la misma acción siempre.
    board.make_action(board.random_action());
    board.make_action(board.random_action());

    vertex action;

    for (int i = 0; i < 60 && !board.is_complete(); ++i)
    {
        auto action_set = board.get_available_sample_cells(1.0);

        // Caso de que no haya una accion para hacer.
        if (action_set.size() == 1 && action_set[0] == -1)
        {
            board.make_action(-1);
            continue;
        }

        if (first_to_play == board.player_status())
            action = player1.search(board);
        else
            action = player2.search(board);

        board.make_action(action);
    }

    return board.reward(second_to_play) < board.reward(first_to_play);
}

template <class search_algorithm1, class search_algorithm2>
double evaluate_accuracy(search_algorithm1& player1,
                         search_algorithm2& player2,
                         BoardGame& board,
                         int games_played)
{
    double win_rate = 0.0;
    std::cout
      << "Evaluamos el número de veces que gana el algoritmo 1 contra el otro."
      << std::endl;
    for (int i = 0; i < games_played; ++i)
    {
        std::cout << "Game number " << i << std::endl;
        if (is_winner(player1, player2, board))
            win_rate++;
    }

    win_rate *= (1.0/games_played);
    return win_rate;
}
