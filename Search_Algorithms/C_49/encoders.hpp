#pragma once
#include "../../Include/Extra/Graph.hpp"
#include <iostream>
#include <torch/torch.h>
#include <tuple>
#include <vector>

using vertex = int;
using example = torch::data::Example<torch::Tensor, torch::Tensor>;
/*
Crea tensor de 4 x número de vertices x número de vertices.
- Primer  plano: piezas del jugador actual.
- Segundo plano: piezas del adversario.
- Tercer  plano: acciones validas en el estado actual.
*/
/*
-----------------------------------------------------------
Encoder para redes convolucionales 2d.
-----------------------------------------------------------
*/
class SimpleEncoder2d
{
public:
    SimpleEncoder2d(){}

    SimpleEncoder2d(const Graph& BG) : G(BG) {}

    // Solo funciona si le mandas todos los estados del juego. (Mira la
    // estrucutra en game_structure.hpp)
    template <class Game>
    std::vector<example> Encode_episode(const Game& G)
    {
        std::vector<example> game_recodings;
        int reward = G.black_reward;
        int current_player = 0; // Zero represent first player.

        for (int i = 0; i < G.episode_size(); ++i)
        {
            auto data =
              Encode_data(G.states[i], G.valid_actions[i], current_player);
            
            auto target = Encode_label(reward, G.probabilities[i]);
            
            game_recodings.emplace_back(data, target);

            reward *= -1;
            current_player ^= 1;
        }

        return game_recodings;
    }

    // This two parameters give you the current state.
    torch::Tensor Encode_data(const std::vector<char>& state,
                              const std::vector<vertex>& valid_actions,
                              int player)
    {
        set_player_planes();
        Update_pieces(state);

        torch::Tensor data = torch::zeros({3, G.num_vertices(), G.num_vertices()});

        std::vector<double> valid_actions_plane(G.num_vertices() *
                                                  G.num_vertices(),
                                                0);

        for (auto v : valid_actions)
        {
            if(v != -1)
                update_action_to(valid_actions_plane, v, 1);
        }

        torch::Tensor plane_white =
          torch::tensor(AdjMatrix_White).view({G.num_vertices(), G.num_vertices()});
        torch::Tensor plane_black =
          torch::tensor(AdjMatrix_Black).view({G.num_vertices(), G.num_vertices()});
        torch::Tensor plane_valid_actions = torch::tensor(valid_actions_plane)
                                              .view({G.num_vertices(),
                                                     G.num_vertices()});

        int ind_white_ = 1;
        int ind_black_ = 0;
        // When player is 1 then first plane is white.
        if (player)
        {
            ind_white_ = 0;
            ind_black_ = 1;
        }

        data[ind_white_] = plane_white;
        data[ind_black_] = plane_black;
        data[2] = plane_valid_actions;

        return data;
    }

    torch::Tensor Encode_label(int reward, std::vector<double> prob)
    {
        double reward_ = (double)reward;
        prob.push_back(reward_);

        torch::Tensor target = torch::tensor(prob);

        return target;
    }

private:
    torch::Tensor Planes;
    Graph G;
    std::vector<double> AdjMatrix_White;
    std::vector<double> AdjMatrix_Black;

    void set_player_planes()
    {
        AdjMatrix_Black.clear();
        AdjMatrix_White.clear();

        AdjMatrix_White.resize(G.num_vertices()*G.num_vertices(), 0);
        AdjMatrix_Black.resize(G.num_vertices()*G.num_vertices(), 0);
    }

    void Update_pieces(const std::vector<char>& state)
    {
        int leght = state.size();
        for (int v = 0; v < leght; ++v)
        {
            if (state[v] == 'B')
                update_action_to(AdjMatrix_Black, v, 1);
            else if(state[v] == 'W')
                update_action_to(AdjMatrix_White, v, 1);
        }
    }

    // Si queremos pesar (?)
    void update_action_to(std::vector<double>& array, int vertex_, int value)
    {
        int ind = vertex_ * G.num_vertices();
        for (auto v : G.neighbors(vertex_))
            array[ind + v] = value;
    }
};

/*
---------------------------------------------------------
Encoder con planos en 1d.
---------------------------------------------------------
*/

class SimpleEncoder1d
{
public:
    SimpleEncoder1d(){}

    SimpleEncoder1d(const Graph& BG) : G(BG) {}

    // Solo funciona si le mandas todos los estados del juego. (Mira la
    // estrucutra en game_structure.hpp)
    template <class Game>
    std::vector<example> Encode_episode(const Game& G)
    {
        std::vector<example> game_recodings;
        int reward = G.black_reward;
        int current_player = 0; // Zero represent first player.

        for (int i = 0; i < G.episode_size(); ++i)
        {
            auto data =
              Encode_data(G.states[i], G.valid_actions[i], current_player);
            
            auto target = Encode_label(reward, G.probabilities[i]);
            
            game_recodings.emplace_back(data, target);

            reward *= -1;
            current_player ^= 1;
        }

        return game_recodings;
    }

    // This two parameters give you the current state.
    torch::Tensor Encode_data(const std::vector<char>& state,
                              const std::vector<vertex>& valid_actions,
                              int player,
                              bool is_one_sample = false
                              )
    {
        set_player_planes();
        Update_pieces(state);

        torch::Tensor data = torch::zeros({3,G.num_vertices()* G.num_vertices()});

        std::vector<double> valid_actions_plane(G.num_vertices() *
                                                  G.num_vertices(),
                                                0);

        for (auto v : valid_actions)
        {
            if(v != -1)
                update_action_to(valid_actions_plane, v, 1);
        }

        torch::Tensor plane_white = torch::tensor(AdjMatrix_White);
        torch::Tensor plane_black = torch::tensor(AdjMatrix_Black);
        torch::Tensor plane_valid_actions = torch::tensor(valid_actions_plane);        

        int ind_white_ = 1;
        int ind_black_ = 0;
        // When player is 1 then first plane is white.
        if (player)
        {
            ind_white_ = 0;
            ind_black_ = 1;
        }

        data[ind_white_] = plane_white;
        data[ind_black_] = plane_black;
        data[2] = plane_valid_actions;

        if(is_one_sample)
        {
            //sample num , channels , sequence length.
            data = data.view({1,3,G.num_vertices() * G.num_vertices()});
        }
        return data;
    }

    torch::Tensor Encode_label(int reward, std::vector<double> prob)
    {
        double reward_ = (double)reward;
        prob.push_back(reward_);

        torch::Tensor target = torch::tensor(prob);

        return target;
    }

private:
    torch::Tensor Planes;
    Graph G;
    std::vector<double> AdjMatrix_White;
    std::vector<double> AdjMatrix_Black;

    void set_player_planes()
    {
        AdjMatrix_Black.clear();
        AdjMatrix_White.clear();

        AdjMatrix_White.resize(G.num_vertices()*G.num_vertices(), 0);
        AdjMatrix_Black.resize(G.num_vertices()*G.num_vertices(), 0);
    }

    void Update_pieces(const std::vector<char>& state)
    {
        int leght = state.size();
        for (int v = 0; v < leght; ++v)
        {
            if (state[v] == 'B')
                update_action_to(AdjMatrix_Black, v, 1);
            else if(state[v] == 'W')
                update_action_to(AdjMatrix_White, v, 1);
        }
    }

    // Si queremos pesar (?)
    void update_action_to(std::vector<double>& array, int vertex_, int value)
    {
        int ind = vertex_ * G.num_vertices();
        for (auto v : G.neighbors(vertex_))
            array[ind + v] = value;
    }
};