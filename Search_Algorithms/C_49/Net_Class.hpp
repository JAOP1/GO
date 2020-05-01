#pragma once
#include <torch/torch.h>

struct Network_evaluator : nn::Module
{
    Network_evaluetor(int input_size)
        : conv1(nn::ConvTranspose2dOptions(kNoiseSize, 256, 4).bias(false))
        , batch_norm1(256)
        , conv2(
            nn::ConvTranspose2dOptions(256, 128, 3).stride(2).padding(1).bias(false))
        , batch_norm2(128)
        , conv3(
            nn::ConvTranspose2dOptions(128, 64, 4).stride(2).padding(1).bias(false))
        , batch_norm3(64)
        , conv4(nn::ConvTranspose2dOptions(64, 1, 4).stride(2).padding(1).bias(
            false))
    {
        // register_module() is needed if we want to use the parameters() method
        // later on
        register_module("conv1", conv1);
        register_module("conv2", conv2);
        register_module("conv3", conv3);
        register_module("conv4", conv4);
        register_module("batch_norm1", batch_norm1);
        register_module("batch_norm2", batch_norm2);
        register_module("batch_norm3", batch_norm3);
    }

    torch::Tensor forward(torch::Tensor x)
    {
        x = torch::LeakyRELU(batch_norm1(conv1(x)));
        x = torch::LeakyRELU(batch_norm2(conv2(x)));
        x = torch::LeakyRELU(batch_norm3(conv3(x)));
        x = torch::Sigmoid(conv4(x));
        return x;
    }

    nn::ConvTranspose2d conv1, conv2, conv3, conv4;
    nn::BatchNorm2d batch_norm1, batch_norm2, batch_norm3;
};

TORCH_MODULE(Network_evaluetor);
