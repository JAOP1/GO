#pragma once

#include "NumberTheory.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
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

double distance(const sf::Vector2i& A, const sf::Vector2i& B)
{
    double X = std::pow(A.x - B.x, 2);
    double Y = std::pow(A.y - B.y, 2);

    return std::sqrt(X + Y);
}

template <class T>
struct polynomial_hash
{
    using ull = unsigned long long;
    using ll = long long;

    ull operator()(std::vector<T> const& pieces) const
    {
        ll x = 31;
        ll p = 1000000007;
        ll result = 0;

        for (ll v = 0; v < pieces.size(); ++v)
        {
            ll color = 0;
            if (pieces[v] == 'B')
                color = 2;
            else if (pieces[v] == 'W')
                color = 1;

            result *= x;
            result += color;
            reduce_mod(result, p);
        }

        return result;
    }
};
