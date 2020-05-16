#pragma once 
#include "../../Include/Extra/Graph.hpp"
#include <torch/torch.h>
#include <iostream>
#include <vector>
#include <tuple>


using vertex = std::int64_t;
using example =  torch::data::Example<torch::Tensor , torch::Tensor>;
/*
Crea tensor de 4 x número de vertices x número de vertices.
- Primer  plano: piezas del jugador actual.
- Segundo plano: piezas del adversario. 
- Tercer  plano: acciones validas en el estado actual.
*/

class SimpleEncoder
{
    public:

        SimpleEncoder(const Graph& BG) : G(BG)
        {}
        
        //Solo funciona si le mandas todos los estados del juego. (Mira la estrucutra en loading_data.hpp)
        template<class Game>
        std::vector<example> Encode_episode(const Game& G )
        {
            std::vector<example> game_recodings;
            int reward = G.black_reward;
            int current_player = 0; //Zero represent first player.

            for(int i = 0; i < G.episode_size() ; ++i)
            {
                auto data = Encode_data(G.states[i], G.valid_actions[i] , current_player);
                auto target = Encode_label(reward , G.probabilities[i]);
                game_recodings.emplace_back(data,target);

                reward *= 1; 
                current_player ^= 1;
            }

            return game_recodings;
        }

        //This two parameters give you the current state.
        torch::Tensor Encode_data(const std::vector<char>& state,const std::vector<vertex>& valid_actions ,int player)
        {
            set_player_planes();
            Update_pieces(state);

            torch::Tensor data = torch::zeros({4,G.num_vertices(), G.num_vertices()});

            std::vector<double> valid_actions_plane(G.num_vertices() * G.num_vertices(),0);

            for(auto v :  valid_actions)
            {
                update_action_to(valid_actions_plane , v , 1);
            }

            torch::Tensor plane_white = torch::tensor(AdjMatrix_White).view({G.num_vertices() , G.num_vertices()});
            torch::Tensor plane_black = torch::tensor(AdjMatrix_Black).view({G.num_vertices() , G.num_vertices()});
            torch::Tensor plane_valid_actions = torch::tensor(valid_actions_plane).view({G.num_vertices() , G.num_vertices()});

            data[0] = plane_white;
            data[1] = plane_black;
            data[2] = plane_valid_actions;

            return data;            
        }

        torch::Tensor Encode_label(int reward , std::vector<double> prob)
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

            AdjMatrix_White.resize(G.num_vertices()* G.num_vertices() , 0);
            AdjMatrix_Black.resize(G.num_vertices() * G.num_vertices() , 0 );

        }

        void Update_pieces(const std::vector<char>& state)
        {

            for(int v = 0 ;  v < state.size() ; ++v)
            {

                if(state[v] == 'N')
                {
                    update_action_to(AdjMatrix_White , v , 0);
                    update_action_to(AdjMatrix_Black, v, 0);
                }
                else if( state[v] == 'B')
                    update_action_to(AdjMatrix_Black, v , 1);
                else         
                    update_action_to(AdjMatrix_White, v, 1);
        
            }
        }

        //Si queremos pesar (?)
        void update_action_to(std::vector<double>& array , int vertex_ , int value)
        {
            int ind = vertex_ * G.num_vertices();
            for(auto v : G.neighbors(vertex_))
                array[ind + v] = value;
            
        }

};