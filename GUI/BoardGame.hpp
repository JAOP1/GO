#pragma once

#include "Include/Extra/DisjointSets.hpp"
#include "Include/Extra/Graph.hpp"
#include "Include/Extra/hash_utilities.hpp"
#include <cstdlib>
#include <limits>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <vector>
#include <functional>
#include <random>

using size_type = std::int64_t;
using index_type = std::int64_t;
using vertex = std::int64_t;
using player = char;

std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(0.0,1.0);

struct board_node
{

    void reset()
    {
        liberties = 0;
        libertiesNodes.clear();
        type = 'N';
    }

    std::set<vertex> libertiesNodes;
    int liberties = 0;
    int eyes_by_group = 0;
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
    BoardGame(Graph& G)
        : Board(G)
        , pieces(G.num_vertices())
        , groups(G.num_vertices())
    {}

    char player_status() const { return players[current_player]; }

    std::vector<char> show_current_state() const;
    
    vertex parent_node(vertex node) const { return groups.find_root(node); }

    std::vector<vertex> get_available_sample_cells( double portion) const;
    
    int reward(char player) const;

    bool is_complete() const; //Falta...

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

    //Complement functions by main functions.
    void Put_piece(vertex node);

    std::vector<int> dead_vertices(vertex node); 

    // Y empty cells on the group X.
    DominatedCells count_dominated_region(vertex v, std::vector<bool>& is_visited_global) const;
  
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

    bool is_frontier(vertex v, const std::vector<bool>& visited) const
    {
        for (auto v : Board.neighbors(v))
        {
            if (!visited[v])
                return true;
        }

        return false;
    }

    bool is_alive_group(vertex v) const ;


    //Variables of game 
    std::vector<player> players = {'B', 'W'};
    int current_player = 0; 
    std::vector<bool> resigned_player = {false,false}; //Si pasó un jugador.
    std::vector<board_node> pieces; //Piezas en el tablero.
    disjoint_sets groups;
    
   
};

// Public functions
// -----------------------------------------------------------------------------------

std::vector<char> BoardGame::show_current_state() const
    {
        std::vector<char> type_cell(pieces.size());
        std::transform(pieces.begin(),
                       pieces.end(),
                       type_cell.begin(),
                       [](const board_node& x)->char { return x.type; });

        return type_cell;
    }

std::vector<vertex> BoardGame::get_available_sample_cells(double portion ) const 
    {
        
        std::vector<vertex> cells;
        for (vertex v = 0; v < Board.num_vertices(); ++v)
        {
            if (pieces[v].type == 'N' && is_valid_move(v) && distribution(generator) <= portion)
                cells.push_back(v);
        }

        //Accion de resignar la aniadiremos a las celdas disponibles.
        //cells.push_back(-1);
        return cells;
    }



std::vector<int> BoardGame::make_action(vertex v)  
{
    //Cuando un jugador pasa.
    if(v == -1)
    {
        resigned_player[current_player]  = true;
        current_player ^= 1;
        return {}; 
    }

    resigned_player[0] = false;
    resigned_player[1] = false;
    Put_piece(v);
    current_player ^= 1;
    return  dead_vertices(v);

    
}

void BoardGame::undo_action(vertex v) 
{
    // TODO: Implementar
}

int BoardGame::reward(char player) const
{
    std::vector<bool> visited(Board.num_vertices() , false);
    int dominated_by_black = 0;
    int dominated_by_white = 0;

    for(vertex v = 0 ; v < Board.num_vertices() ; ++v)
    {
        if(!visited[v] && pieces[v].type == 'N')
        {
            DominatedCells result = count_dominated_region(v , visited);

            if(result.player == 'B')
            {
                dominated_by_black += result.num_dominated;
                continue;
            }
            dominated_by_white += result.num_dominated;
        }
    }

    if(player == 'B')
        return dominated_by_black;

    return dominated_by_white;
}



bool BoardGame::is_complete() const
{
    return (resigned_player[0] && resigned_player[1]) ;
}


/*
Un movimiento es valido, si es posible colocar y no se suicida.
*/
bool BoardGame::is_valid_move(vertex v) const
{
    if(v == -1)
        return true;
    //Esta posicion ya está ocupada.
    if (pieces[v].type != 'N')
        return false;

    //Revisar si no es un grupo seguro.
    if(is_alive_group(v))
        return false;

    auto type = players[current_player];
    std::set<vertex> empty_cells;
    std::set<vertex> parents;
    int min_liberties_opponent = std::numeric_limits<int>::max();

    for (auto neighbor : Board.neighbors(v))
    {
        //Revisar posiciones sin ficha.
        if (pieces[neighbor].type == 'N')
            empty_cells.insert(neighbor);

        else
        {
            vertex parent = groups.find_root(neighbor);
            //Ver si es posible quitar un grupo del otro jugador.
            if(pieces[neighbor].type != type)
                min_liberties_opponent = std::min(min_liberties_opponent , liberties(neighbor));

            //Unir grupos del mismo tipo, para contar celdas vacias.
            else if(parents.find(parent) == parents.end() )//!groups.are_in_same_connected_component(v,neighbor))
            {
                empty_cells.insert(pieces[parent].libertiesNodes.begin(),
                            pieces[parent].libertiesNodes.end());
                parents.insert(parent);         
            }
        }
    }

    min_liberties_opponent--;
    empty_cells.erase(v);
    //Si al colocar la ficha nueva, tiene más grados de libertad
    // o algun grupo pierde todos sus grados de libertad. 
    //En ese caso significa que se estaría suicidando. 
    if (!empty_cells.empty() || min_liberties_opponent == 0 )
        return true;

    return false;
}

// If not available action in the current board.
vertex BoardGame::random_action() const
{
    auto actions_set = get_available_sample_cells(1.0);
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

    for (auto v : Board.neighbors(node))
    {
        //Quiero agregar las casillas vacias a mi grupo de libertades.
        if(pieces[v].type == 'N')
            pieces[node].libertiesNodes.insert(v);
        else
        {
            //Los grupos de fichas opuestas, borrar ese grado de libertad
            if(pieces[v].type != type)
            {
                auto parent = groups.find_root(v);
                pieces[parent].libertiesNodes.erase(node);
            }
            //Unir con los del mismo tipo de fichas.
            else if(!groups.are_in_same_connected_component(v,node))
            {
                auto old_parent = groups.find_root(v);
                groups.merge(node, v);
                
                pieces[node].libertiesNodes.insert(pieces[old_parent].libertiesNodes.begin(),
                                                   pieces[old_parent].libertiesNodes.end());
            }   
        }
    }

    //El nuevo nodo agregado es el padre de todo.
    //Tenemos que eliminarlo porque antes era una casilla vacia.
    pieces[node].libertiesNodes.erase(node);
}


std::vector<int> BoardGame::kill_group(vertex node)
{
    std::vector<int> array_nodes;

    vertex root = groups.find_root(node);

    for (int i = 0; i < Board.num_vertices(); ++i)
        groups.find_root(i);

    vertex node_ = 0;

    for (auto v : groups.parents())
    {

        if (v == root)
        {
            //Agrega la pieza desplazada del tablero 
            //al grupo de fichas del adversario como grado de libertad.
            for (auto neighbor : Board.neighbors(node_))
            {
                if (pieces[node_].type != pieces[neighbor].type &&
                    pieces[neighbor].type != 'N' )
                {
                    vertex current = groups.find_root(neighbor);
                    pieces[current].libertiesNodes.insert(node_);
                }
            }

            groups.reset_parent(node_);
            pieces[node_].reset();

            array_nodes.push_back(node_);
        }
        node_++;
    }

    return array_nodes;
}

std::vector<int> BoardGame::dead_vertices(vertex node)  
{
    std::vector<int> array_nodes;
    for (vertex v : Board.neighbors(node))
    {
        //Revisar esta parte, que está haciendo cosas extrañas.
        if (pieces[v].type != 'N' && pieces[v].type != pieces[node].type && is_dead(v) )
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

    update_liberties(node, type);

    
}


DominatedCells BoardGame::count_dominated_region(vertex node, std::vector<bool>& is_visited_global) const
{
    std::vector<bool> is_visited(Board.num_vertices(), false);
    std::vector<vertex> walls;
    std::stack<vertex> frontier;

    frontier.push(node);
    int count_empty_cells = 0;

    while (!frontier.empty())
    {
        vertex top = frontier.top();
        frontier.pop();

        is_visited[top] = true;
        is_visited_global[top] = true;
    
        if (pieces[top].type != 'N' )
        {
            walls.push_back(top);
            continue;
        }
        
        count_empty_cells++;

        for (auto neighbor : Board.neighbors(top))
        {
            if(!is_visited[neighbor])
                frontier.push(neighbor);
        }

    }

    char group = 'N';
    //Revisa quien es el jugador que domina esa zona.
    for (auto v : walls)
    {
        if (is_frontier(v, is_visited))
        {
            group = pieces[v].type;
            break;
        }
    }

    //Revisa si todas las fichas que rodean esa parte son del mismo tipo.
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

bool BoardGame::is_alive_group(vertex v) const
{
    
    std::function<bool (vertex)> is_eye = [&](vertex v)
    {
        auto neighbors = Board.neighbors(v);
        vertex label_ = groups.find_root(neighbors[0]);

        for(auto neighbor :  neighbors)
        {
            //Nos interesan los ojos de nuestro color.
            if(pieces[neighbor].type != players[current_player])
            {
                return false;
            }

            // Aqui indica si corresponden el mismo conjunto de piezas al mismo grupo.
            vertex label_group = groups.find_root(neighbor); 
            if(label_group != label_)
                return false;
            
        }

        return true;
    };


    if(!is_eye(v))
        return false;
    
    //Revisar posibles candidatos de segundo ojo.
    auto neighbors = Board.neighbors(v);
    vertex label_ = groups.find_root(neighbors[0]);
    std::set<vertex>::iterator it = pieces[label_].libertiesNodes.begin();

    for(it ; it != pieces[label_].libertiesNodes.end() ; ++it)
    {
        vertex cell = *it;
        if(cell == v)
            continue;

        if(is_eye(cell))
            return true;
    }
    return false;
}