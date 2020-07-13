#pragma once
#include "../torch_utils.hpp"
#include <string>
#include <torch/torch.h>
#include <vector>

using namespace torch::indexing;

struct GridNetwork : torch::nn::Module
{
    GridNetwork(nn_utils::neural_options& options) : options_(options)
    {
        // Create convolutional layers.
        for (int i = 0; i < options_.num_convlayers(); ++i)
        {
            auto options_layer = options_.get_options_convlayer(i);
            conv_layers.emplace_back(
              torch::nn::Conv2dOptions(options_layer.in_channels,
                                       options_layer.out_channels,
                                       options_layer.kernel_size)
                .padding(1));
            register_module("conv" + std::to_string(i), conv_layers[i]);
        }
        std::cout << "total convolutional layers created: " << conv_layers.size()
                  << std::endl;
        // Create linear layers.
        for (int i = 0; i < options_.num_linearlayers(); ++i)
        {
            auto options_layer = options_.get_options_linearlayer(i);
            linear_layers.emplace_back(options_layer.input, options_layer.output);
            register_module("linear" + std::to_string(i), linear_layers[i]);
        }
        std::cout << "total linear layers created: " << linear_layers.size()
                  << std::endl;
        auto options_llayer = options_.get_options_linearlayer(0);
        num_conv = conv_layers.size();
        num_linear = linear_layers.size();
        options_llayer = options_.get_options_linearlayer(num_linear - 1);
        output_size = options_llayer.output;
    }

    torch::Tensor forward(torch::Tensor x)
    {
        // x = torch::relu(conv_layers[0]->forward(x));
        // std::cout<<x.sizes()<<std::endl;

        for (int i = 0; i < num_conv; ++i)
        {
            x = conv_layers[i]->forward(x);

            if (options_.pool_layer(i))
            {
                std::cout << "Max pool in " << i << " layer." << std::endl;
                x = torch::max_pool2d(x, 2);
            }

            x = torch::relu(x);

            // std::cout<<x.sizes()<<std::endl;
        }

        // Flattening.
        int num_samples = x.sizes()[0];
        x = x.view({num_samples, -1});
        // std::cout<<x.sizes()<<std::endl;

        // The last one is different process.
        for (int i = 0; i < num_linear - 1; ++i)
        {
            x = torch::relu(linear_layers[i]->forward(x));
            x = torch::dropout(x, /*p=*/0.5, /*training*/ is_training());
        }

        x = linear_layers[num_linear - 1]->forward(x);
        // Esto sería para una policy and value network.
        // x.index_put_({Slice(), Slice(None, output_size - 1)},
        //              torch::softmax(
        //                x.index({Slice(), Slice(None, output_size -
        //                1)}).clone(), -1));

        // x.index_put_({Slice(), output_size - 1},
        //              torch::tanh(x.index({Slice(), output_size -
        //              1}).clone()));

        /*
        This will give us a vector with leght of all actions (include pass)
        and prediction about state.
        */
        x = torch::tanh(x);
        return x;
    }

    nn_utils::neural_options options_;
    std::vector<torch::nn::Conv2d> conv_layers;
    std::vector<torch::nn::Linear> linear_layers;
    int num_conv = 0;
    int num_linear = 0;
    int output_size = 0;
};