#pragma once 
#include "../C_49/Net_trainer.hpp"
#include "../C_49/Include/Data_manage.hpp"
#include <torch/torch.h>

template< class NN, class encoder>
void for_train(NN& Model, encoder& Encoder_, std::string DataPath, int n)
{
    std::cout<< "Estas en funcion for train."<<std::endl;
    torch::optim::Adam optimizer(Model.parameters(),
                                 torch::optim::AdamOptions(0.001));
    std::cout<<"Aqui sigue funcionando 1"<<std::endl;
    auto data = get_data_games<encoder>(/*Path_to_load=*/DataPath,
                                            /*Encoder= */ Encoder_); 
    std::cout<<"Aqui sigue funcionando 2"<<std::endl;
    auto Dataset = GameDataSet(data).map(
        torch::data::transforms::Stack<>()); // Transform the dataset in
                                            // understable form for torch.
    std::cout<<"Aqui sigue funcionando 3"<<std::endl;
    torch::data::samplers::RandomSampler sampler(data.size()); // This say
                                                                // how will
                                                                // be getting
                                                                // the data.
    std::cout<<"Aqui sigue funcionando 4"<<std::endl;
    auto DataLoader =
        torch::data::make_data_loader(Dataset, sampler, 110);
    std::cout<<"Estoy a punto de entrar al ciclo!(dark)"<<std::endl;          
    for(int i = 0;  i < n ; ++i)
    {
        train_one_epoch(Model, *DataLoader, optimizer, data.size());
    }
}