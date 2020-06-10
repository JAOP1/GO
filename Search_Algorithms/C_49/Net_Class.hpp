#pragma once
#include <torch/torch.h>
#include "torch_utils.hpp"
#include <vector>
#include <string>
/*
Esta es una red neuronal convolucional 1d para cualquier grafo.
Nota: tienes que especifiar el número de capas.
*/
using namespace torch::indexing;
//using json = nlohmann::json;

struct Network_evaluator : torch::nn::Module
{
  Network_evaluator(nn_utils::neural_options& options)
  {
      //Create convolutional layers.
      for(int i = 0; i < options.num_convlayers(); ++i)
      {
          auto options_layer = options.get_options_convlayer(i);
          conv_layers.emplace_back(torch::nn::Conv1dOptions(options_layer.in_channels,
                                                            options_layer.out_channels,
                                                            options_layer.kernel_size));
          register_module("conv" + std::to_string(i), conv_layers[i]);
      }
      std::cout<<"total convolutional layers created: "<<conv_layers.size()<<std::endl;
      //Create linear layers.
      for(int i = 0; i < options.num_linearlayers(); ++i)
      {
          auto options_layer = options.get_options_linearlayer(i);
          linear_layers.emplace_back(options_layer.input,
                                      options_layer.output);
          register_module("linear"+std::to_string(i), linear_layers[i]);
      }
      std::cout<<"total linear layers created: "<<linear_layers.size()<<std::endl;
      auto options_llayer = options.get_options_linearlayer(0);
      flatten_to = options_llayer.input;
      num_conv = conv_layers.size();
      num_linear = linear_layers.size();
      options_llayer = options.get_options_linearlayer(num_linear-1);
      output_size = options_llayer.output;
  }

  
  torch::Tensor forward(torch::Tensor x)
  {
      for(int i = 0;  i < num_conv; ++i)
      {
          x = torch::relu(conv_layers[i]->forward(x));
          //std::cout<<x.sizes()<<std::endl;
      }

      //Flattening.
      int num_samples = x.sizes()[0];
      x = x.view({num_samples,flatten_to});
      //std::cout<<x.sizes()<<std::endl;
      
      //The last one is different process.
      for(int i = 0; i < num_linear-1; ++i)
      {
          x = torch::relu(linear_layers[i]->forward(x));
          x = torch::dropout(x , /*p=*/0.5 , /*training*/is_training());
      }
      
      x = linear_layers[num_linear-1]->forward(x);
      x.index_put_({Slice() , Slice(None, output_size - 1)}, 
      torch::softmax(x.index({Slice() , Slice(None, output_size-1)}),-1));

      x.index_put_({Slice(), output_size-1},
      torch::tanh(x.index({Slice() , output_size-1})));
      
      /*
      This will give us a vector with leght of all actions (include pass)
      and prediction about state.
      */
      return x;
  }

  std::vector<torch::nn::Conv1d> conv_layers;
  std::vector<torch::nn::Linear> linear_layers; 
  int flatten_to=0;
  int num_conv=0;
  int num_linear=0;
  int output_size = 0;
};
//TORCH_MODULE(Network_evaluetor);

