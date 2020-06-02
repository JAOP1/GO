#pragma once
#include <torch/torch.h>


/*
Esta es una red neuronal convolucional 1d para un grafo de 25 nodos.
Por hacer una red mas generica!!
*/
struct Network_evaluator : torch::nn::Module
{
    //Ahorita haré una red prueba... para un grid 5x5.
    Network_evaluator(int valor):
        /*Lout = (Lin - (kernel_size)) + 1*/
        conv1( torch::nn::Conv1dOptions(/*in_channels=*/3,/*out_channels=*/ 5,/*kernel_size=*/ 3)),
        /*Output conv1 = {num_samples , 5 ,23}*/
        /*Maxpool(conv1) = {num_samples, 5, 12}*/
        conv2(torch::nn::Conv1dOptions(5 , 10 , 3)),
        /*Output con2 = {num_samples, 10, 10}*/
        fc1(3090 , 1540),
        fc2(1540,360),
        fc3(360,25)
    {
        // register_module() is needed if we want to use the parameters() method
        // later on
        register_module("conv1", conv1);
        register_module("conv2", conv2);
        register_module("fc1", fc1);
        register_module("fc2", fc2);
        register_module("fc3",fc3);
    }
    
    torch::Tensor forward(torch::Tensor x)
    {
        x = torch::relu(torch::max_pool1d(conv1->forward(x), 2));
        std::cout<<x.sizes()<<std::endl;
        x = torch::relu(conv2->forward(x));
        std::cout<<x.sizes()<<std::endl;
        
        int num_samples = x.sizes()[0];
        x = x.view({num_samples,3090});
        std::cout<<x.sizes()<<std::endl;
        x = torch::relu(fc1->forward(x));
        x = torch::dropout(x, /*p=*/0.5, /*training=*/is_training());
        x = torch::relu(fc2->forward(x));
        x = torch::dropout(x, 0.5, is_training());
        x = fc3->forward(x);
        return torch::softmax(x,1);
    }

    torch::nn::Conv1d conv1,conv2;
    torch::nn::Linear fc1 ,fc2,fc3;

};
//TORCH_MODULE(Network_evaluetor);
