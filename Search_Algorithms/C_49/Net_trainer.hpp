#pragma once
#include <torch/torch.h>
#include "UCT_Net.hpp"
#include "RAVE_Net.hpp"
#include "Net_Class.hpp"
#include "encoders.hpp"
#include "Data_manage.hpp"
#include <string>
#include <filesystem>


size_t dataset_size;


template<class Net_archictec>
void load_net(std::string Path , Net_archictec& Model)
{
    if (std::filesystem::exists(Path))
    {
        auto load_model_ptr = std::make_shared<Net_archictec>();
        torch::load(load_model_ptr, Path);
        Model = *load_model_ptr;
    }
}

template<class Net_archictec>
void save_net(std::string Path , Net_archictec& Model)
{
    auto save_model_ptr = std::make_shared<Net_archictec>(Model);
    torch::save(save_model_ptr, Path);
}

// Aqui probar cosas. (Falta por hacer!)
template <typename loss>
loss calculate_loss(torch::Tensor& data, torch::Tensor target)
{
    auto output1 = torch::nll_loss(data, target);
    return output1;
}

template <typename DataLoader>
void train_one_epoch(Network_evaluator& model,
                     DataLoader& loader,
                     torch::Device device,
                     torch::optim::Optimizer& optimizer)
{

    model.train();
    size_t batch_idx = 0;
    for (auto& batch : loader)
    {
        auto data = batch.data.to(device);
        auto targets = batch.target.to(device);
        optimizer.zero_grad();
        torch::Tensor output = model.forward(data);
        torch::Tensor loss = calculate_loss<torch::Tensor>(output, targets);
        loss.backward();
        optimizer.step();

        if (batch_idx++%4 == 0)
        {
            std::printf("\rTrain Epoch: [%5ld/%5ld] Loss: %.4f",
                        batch_idx*batch.data.size(0),
                        dataset_size,
                        loss[0].item<double>());
        }
    }
}


//Parcialmente completo.
template<class search_type,class Encoder>
void train_model(std::string ModelPath,
                 std::string DataPath,
                 BoardGame& G,
                 int games,
                 int Batch_size,
                 int Num_epoch,
                 torch::Device device)
{
    Network_evaluator Model;
    Network_evaluator Model_tmp;
    
    load_net<Network_evaluator>(ModelPath , Model);
    load_net<Network_evaluator>(ModelPath, Model_tmp);

    Model.to(device);
    Model_tmp.to(device);
    Model_tmp.eval();

    Encoder Encoder_(G.Board);
    torch::optim::SGD optimizer(Model.parameters(), torch::optim::SGDOptions(0.01).momentum(0.5));

    // Condición de paro. (Falta por hacer!)
    int valor_ini = 2;
    while (valor_ini--)
    {
        std::cout<<"Generando juegos."<<std::endl;
        generate_games<search_type,Encoder>(/*Path_to_save = */ DataPath,
                       /*total_records = */ games,
                       /*Model = */ Model_tmp,
                       /*BoardGame = */ G,
                       /*Encoder*/ Encoder_);

        std::cout<<"Creando conjunto de datos."<<std::endl;
        auto data = get_data_games<Encoder>(/*Path_to_load=*/DataPath,
                                   /*Encoder= */ Encoder_); // This return a
                                                            // torch example vector.
        auto Dataset = GameDataSet(data).map(
          torch::data::transforms::Stack<>()); // Transform the dataset in
                                               // understable form for torch.

        torch::data::samplers::RandomSampler sampler(
          data.size()); // This say how will be getting the data.
          
        auto DataLoader =
          torch::data::make_data_loader(Dataset, sampler, Batch_size);
          
        std::cout<<"Entrenar red neuronal."<<std::endl;
        for (size_t epoch_ = 1; epoch_ <= Num_epoch; ++epoch_)
            train_one_epoch(Model, *DataLoader, device, optimizer);

        
        search_type current(G , Model , Encoder_, 60 , 'B');
        search_type last( G , Model_tmp , Encoder_ , 60 , 'W');
        std::cout<<"Comparar progreso."<<std::endl;
        if (evaluate_accuracy<search_type , search_type>(current , last, G, 30) >= .6)
        {
            std::cout<<"Obtuvo exito, ha mejorado el modelo"<<std::endl;
            save_net<Network_evaluator>(ModelPath , Model);
            load_net<Network_evaluator>(ModelPath , Model_tmp);
            Model_tmp.to(device);
            Model_tmp.eval();
        }

    }
}