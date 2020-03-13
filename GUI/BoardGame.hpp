#pragma once

#include "Include/Extra/DisjointSets.hpp"
#include "Include/Extra/Graph.hpp"
#include "Include/Extra/hash_utilities.hpp"
#include <cstdlib>
#include <functional>
#include <limits>
#include <random>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <vector>

using size_type = std::int64_t;
using index_type = std::int64_t;
using vertex = std::int64_t;
using player = char;


struct board_node
{

    void reset()
    {
        liberties = 0;
        libertiesNodes.clear();
        is_alive = false;
        type = 'N';
    }

    std::set<vertex> libertiesNodes;
    int liberties = 0;
    bool is_alive = false;
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
    BoardGame(Graph& G, double komi = 0.5)
        : Board(G)
        , pieces(G.num_vertices())
        , groups(G.num_vertices())
        , last_states(2, 0)
        , points_by_white(komi)
        , hashing(G.num_vertices())
    {}

    void debug_function() const;
    
    char player_status() const { return players[current_player]; }

    std::vector<char> show_current_state() const;

    vertex parent_node(vertex node) const { return groups.find_root(node); }

    std::vector<vertex> get_available_sample_cells(double portion) const;

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
        hashing.update(Board.num_vertices());
    }

    Graph Board;

private:
    // Complement functions by main functions.
    void Put_piece(vertex node);

    std::vector<int> dead_vertices(vertex node);

    // Y empty cells on the group X.
    DominatedCells count_dominated_region(
      vertex v, std::vector<bool>& is_visited_global) const;

    void update_liberties(vertex node, char type);

    std::vector<int> kill_group(vertex node);

    bool is_dead(vertex node) const
    {
        auto parent = groups.find_root(node);
        return !liberties(parent);
    }

    int liberties(vertex node) const
    {
        vertex root = groups.find_root(node);
        return pieces[root].libertiesNodes.size();
    }

    bool is_ko(std::vector<vertex>& array_nodes, vertex v) const;

    bool is_frontier(vertex v, const std::vector<bool>& visited) const
    {
        for (auto v : Board.neighbors(v))
        {
            if (!visited[v])
                return true;
        }

        return false;
    }

    void check_alive_groups();
    void find_alive_groups(std::vector<char>& pieces_color_) const;

    // Variables of game
    std::vector<player> players = {'B', 'W'};
    int current_player = 0;
    std::vector<bool> resigned_player = {false, false}; // Si pasó un jugador.
    std::vector<board_node> pieces; // Piezas en el tablero.
    disjoint_sets groups;
    double points_by_white = 0.0;
    double points_by_black = 0.0;

    Zebrist_Hash hashing;
    std::vector<std::uint64_t> last_states;
};
