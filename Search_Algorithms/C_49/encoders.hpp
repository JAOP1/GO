#pragma once 
#include <torch/torch.h>
#include <iostream>
#include <vector>
#include <tuple>
#include "Search_Algorithms/C_49/loading_data.hpp"
#include "Include/BoardGame.hpp"


using element =  torch::data::Example<torch::Tensor , torch::Tensor>;


class SimpleEncoder
{
    public:

        SimpleEncoder(const BoardGame& BG)
        {
            
        }
        

        //Esto permite tener información del juego.
        void set_encoder_planes(const BoardGame& BG)
        {

        }

        //Solo funciona si le mandas todos los estados del juego.
        
        std::vector<element> Encode_episode(const game& G )
        {
            std::vector<element> game_recodings;
            int reward = G.black_reward;

            for(int i = 0; i < G.episode_size() ; ++i)
            {
                auto data = Encode_data(G.actions[i] , G.killed_vertices[i]);
                auto target = Encode_label(reward , G.probabilities[i]);
                game_recodings.emplace_back(data,target);

                reward ^= 1; 
            }

            return game_recodings;
        }

        torch::Tensor Encode_data(int action , std::vector<int>& drop_pieces)
        {

        }

        torch::Tensor Encode_label(int reward , std::vector<double>& prob)
        {

        }
        
    private:

        std::vector<double> AdjMatrix_Black;
        std::vector<double> AdjMatrix_White;


};






