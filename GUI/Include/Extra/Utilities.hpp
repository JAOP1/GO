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
