#pragma once

#include "Include/DisjointSets.hpp"
#include "Include/Extra/Graph.hpp"
#include <cstdlib>
#include <limits>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <vector>

using size_type = std::int64_t;
using index_type = std::int64_t;
using vertex = std::int64_t;
using player = char;

struct stats_node
{

    void reset()
    {
        liberties = 0;
        libertiesNodes.clear();
        type = 'N';
    }

    std::set<vertex> libertiesNodes;
    int liberties = 0;
    char type = 'N';
};

struct DominatedCells
{
    int num_dominated;
    char player;
};

class BoardGame
{
public:
    BoardGame(Graph& G, bool linear_algorithm = false)
        : Board(G)
        , pieces(G.num_vertices())
        , update_by_linear_algorithm(linear_algorithm)
        , groups(G.num_vertices())
    {}

    char player_status() const { return players[current_player]; }

    std::vector<char> show_current_state() const
    {
        std::vector<char> type_cell(pieces.size());
        std::transform(pieces.begin(),
                       pieces.end(),
                       type_cell.begin(),
                       [](const stats_node& x)->char { return x.type; });

        return type_cell;
    }

    vertex parent_node(vertex node) const { return groups.find_root(node); }

    std::vector<vertex> available_cells() const
    {
        std::vector<vertex> result;
        for (vertex v = 0; v < Board.num_vertices(); ++v)
        {
            if (pieces[v].type == 'N' && is_valid_move(v))
                result.push_back(v);
        }
        return result;
    }

    int reward(char player) const;

    bool is_complete() const;

    bool is_valid_move(vertex v) const;

    vertex random_action() const;

    std::vector<int> make_action(vertex v); // Return a vector about the nodes
                                            // killed.
    void undo_action(vertex v); // Falta :C

    void update()
    {

        pieces.clear();
        pieces.resize(Board.num_vertices());

        groups.reset(Board.num_vertices());
    }

    Graph Board;
    
    // Agregagar siguiente linea:
    // Graph Territory
    // Esa se debe actualizar en CADA jugada

private:
    void Put_piece(vertex node);

    std::vector<int> dead_vertices(vertex node) const;

    int liberties(vertex node) const
    {
        vertex root = groups.find_root(node);

        if (update_by_linear_algorithm)
            return pieces[root].liberties;

        return pieces[root].libertiesNodes.size();
    }

    bool is_dead(vertex node) const
    {
        if (!liberties(node))
            return true;

        return false;
    }

    // Y empty cells on the group X.
    // cambia el nombre de la función
    DominatedCells DFS(vertex v, std::vector<bool>& is_visited_global) const
    {
        std::vector<bool> is_visited(Board.num_vertices(), false);
        std::vector<vertex> walls;
        std::stack<vertex> frontier;
        frontier.push(v);
        is_visited[v] = true;
        is_visited_global[v] = true;
        int count_empty_cells = 1;

        while (!frontier.empty())
        {
            vertex top = frontier.top();
            frontier.pop();

            for (auto v : Board.neighbors(top))
            {
                if (pieces[v].type != 'N' && !is_visited[v])
                    walls.push_back(v);

                else if (!is_visited[v])
                {
                    count_empty_cells++;
                    frontier.push(v);
                }
                is_visited[v] = true;
                is_visited_global[v] = true;
            }
        }

        char group = 'N';

        for (auto v : walls)
        {
            if (is_frontier(v, is_visited))
            {
                group = pieces[v].type;
                break;
            }
        }

        for (auto v : walls)
        {
            if (is_frontier(v, is_visited) && group != pieces[v].type)
            {
                group = 'N';
                count_empty_cells = 0;
                break;
            }
        }

        return {count_empty_cells, group};
    }

    bool is_frontier(vertex v, const std::vector<bool>& visited) const
    {
        for (auto v : Board.neighbors(v))
        {
            if (!visited[v])
                return true;
        }

        return false;
    }

    void update_liberties(vertex node, char type);

    // Lineal respecto a los nodos.
    void update_liberties_linear();

    // Lineal respecto a los nodos.
    std::vector<int> kill_group(vertex node);

    bool update_by_linear_algorithm;
    std::vector<player> players = {'B', 'W'};
    int current_player = 0;
    std::vector<stats_node> pieces;
    disjoint_sets groups;
    int count_black;
    int count_white;
};

// Public functions
// -----------------------------------------------------------------------------------

std::vector<int> BoardGame::make_action(vertex v)
{
    Put_piece(v);
    std::vector<int> result = dead_vertices(v);

    return result;
}

void BoardGame::undo_action(vertex v) 
{
    // TODO: Implementar
}

int BoardGame::reward(char player) const
{
    if (player == 'B')
        return count_black;
    return count_white;
}



bool BoardGame::is_complete() const
{
    std::vector<bool> visited(Board.num_vertices(), false);
    bool complete = true;

    for (vertex v = 0; v < Board.num_vertices(); ++v)
    {
        if (visited[v])
            continue;
        
        if (pieces[v].type != 'N')
        {
            DominatedCells result = DFS(v, visited);

            if (result.player == 'N')
                complete = false;
            else if (result.player == 'B')
                count_black = result.num_dominated; // TODO: que se actualice cada que haces movidas
            else
                count_white = result.num_dominated;
        }
    }
    
    return complete;
}

// Solo funciona cuando usamos set.... (Tengo que implementar la otra variante
// D:)
bool BoardGame::is_valid_move(vertex v) const
{
    if (pieces[v].type != 'N')
        return false;

    auto type = players[current_player];
    std::set<vertex> empty_nodes;
    for (auto neighbor : Board.neighbors(v))
    {
        if (pieces[neighbor].type == 'N')
            empty_nodes.insert(neighbor);
    }

    vertex parent;
    int min_liberties = std::numeric_limits<int>::max();

    for (auto neighbor : Board.neighbors(v))
    {
        if (pieces[neighbor].type == type &&
            !groups.are_in_same_connected_component(v, neighbor))
        {
            parent = groups.find_root(neighbor);
            empty_nodes.insert(pieces[parent].libertiesNodes.begin(),
                               pieces[parent].libertiesNodes.end());
        }
        else if (pieces[neighbor].type != type && pieces[neighbor].type != 'N')
        {
            min_liberties = std::min(min_liberties, liberties(neighbor));
        }
    }

    min_liberties--;
    empty_nodes.erase(v);
    if (!empty_nodes.empty() || min_liberties == 0)
        return true;

    return false;
}

// If not available action in the current board.
vertex BoardGame::random_action() const
{
    auto actions_set = available_cells();
    // std::cout<<actions_set.size()<<std::endl;

    if (actions_set.size() == 0)
        return -1;
    int index = rand() % actions_set.size();
    // std::cout<<index<<std::endl;
    return actions_set[index];
}

//---------------------------------------------------------------------------------------------------------
// Private functions

void BoardGame::update_liberties(vertex node, char type)
{
    std::set<vertex> empty_nodes;
    for (auto v : Board.neighbors(node))
    {
        if (pieces[v].type == 'N')
            empty_nodes.insert(v);
    }

    vertex parent;
    for (auto v : Board.neighbors(node))
    {
        if (pieces[v].type == type &&
            !groups.are_in_same_connected_component(v, node))
        {

            parent = groups.find_root(node);
            groups.merge(v, node);
            vertex parent1 = groups.find_root(v);

            pieces[parent1]
              .libertiesNodes.insert(pieces[parent].libertiesNodes.begin(),
                                     pieces[parent].libertiesNodes.end());
        }
        else if (pieces[v].type != type && pieces[v].type != 'N')
        {
            parent = groups.find_root(v);
            pieces[parent].libertiesNodes.erase(node);
        }
    }

    parent = groups.find_root(node);
    pieces[parent].libertiesNodes.erase(node);
    pieces[parent].libertiesNodes.insert(empty_nodes.begin(), empty_nodes.end());
}

void BoardGame::update_liberties_linear()
{

    for (int i = 0; i < Board.num_vertices(); ++i)
        pieces[i].liberties = 0;

    vertex parent;
    for (int i = 0; i < Board.num_vertices(); ++i)
    {

        if (pieces[i].type == 'N')
        {
            std::set<vertex> is_visited;
            int current_size = 0;
            for (auto v : Board.neighbors(i))
            {
                if (pieces[v].type != 'N')
                {
                    parent = groups.find_root(v);
                    is_visited.insert(parent);
                    if (is_visited.size() != current_size)
                    {
                        current_size++;
                        pieces[parent].liberties++;
                    }
                }
            }
        }
    }
}

std::vector<int> BoardGame::kill_group(vertex node)
{
    std::vector<int> array_nodes;

    vertex root = groups.find_root(node);

    for (int i = 0; i < Board.num_vertices(); ++i)
        groups.find_root(i);

    int i = 0;
    std::vector<bool> is_visited(Board.num_vertices(), false);

    for (auto v : groups.parents())
    {
        if (v == root)
        {
            for (auto neighbor : Board.neighbors(i))
            {
                if (pieces[i].type != pieces[neighbor].type &&
                    pieces[neighbor].type != 'N' && !is_visited[neighbor])
                {
                    is_visited[neighbor] = true;
                    vertex current = groups.find_root(neighbor);

                    if (update_by_linear_algorithm)
                        pieces[current].liberties++;
                    else
                        pieces[current].libertiesNodes.insert(i);
                }
            }

            groups.reset_parent(i);
            pieces[i].reset();

            array_nodes.push_back(i);
        }
        i++;
    }

    return array_nodes;
}

std::vector<int> BoardGame::dead_vertices(vertex node)
{
    std::vector<int> array_nodes;
    for (vertex v : Board.neighbors(node))
    {
        if (is_dead(v) && pieces[v].type != 'N' &&
            !groups.are_in_same_connected_component(v, node))
        {
            std::vector<int> new_nodes = kill_group(v);
            array_nodes.insert(array_nodes.end(),
                               new_nodes.begin(),
                               new_nodes.end());
        }
    }

    return array_nodes;
}

void BoardGame::Put_piece(vertex node)
{

    auto type = players[current_player];
    pieces[node].type = type;

    if (update_by_linear_algorithm)
    {
        for (auto v : Board.neighbors(node))
        {
            if (pieces[v].type == type &&
                !groups.are_in_same_connected_component(v, node))
                groups.merge(v, node);
        }

        update_liberties_linear();
    }

    else
        update_liberties(node, type);

    current_player ^= 1;
}
