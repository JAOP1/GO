#pragma once
#include "UCT_Net.hpp"
#include <torch/torch.h>
//#include "RAVE_Net.hpp" //No he implementado nada de RAVE! D:
#include "Data_manage.hpp"
#include "Net_Class.hpp"
#include "encoders.hpp"
#include "torch_utils.hpp"
#include <filesystem>
#include <string>

size_t dataset_size;

// Aqui probar cosas. (Falta por hacer!)
template <typename loss>
loss calculate_loss(torch::Tensor& data, torch::Tensor target)
{
    int policy_vector_leght = target.sizes()[1];
    torch::Tensor policy_target = target.index(
      {Slice(), Slice(None, policy_vector_leght - 1)});
    torch::Tensor value_target = target.index({Slice(), policy_vector_leght - 1});
    torch::Tensor policy_data = data.index(
      {Slice(), Slice(None, policy_vector_leght - 1)});
    torch::Tensor value_data = data.index({Slice(), policy_vector_leght - 1});

    auto output1 = torch::nn::functional::binary_cross_entropy(policy_data,
                                                               policy_target);
    auto output2 = torch::nn::functional::mse_loss(value_data, value_target);
    return output1 + output2;
}

template <typename DataLoader>
void train_one_epoch(Network_evaluator& model,
                     DataLoader& loader,
                     torch::optim::Optimizer& optimizer)
{
    auto device = nn_utils::get_device();

    model.train();
    // size_t batch_idx = 0;
    for (auto& batch : loader)
    {
        auto data = batch.data.to(device);
        auto targets = batch.target.to(device);
        optimizer.zero_grad();
        torch::Tensor output = model.forward(data);
        // std::cout<<"Pasando los datos por el modelo"<<std::endl;
        torch::Tensor loss = calculate_loss<torch::Tensor>(output, targets);
        // std::cout <<"Se ha calculado el error"<<std::endl;
        loss.backward();
        // std::cout<<"backward terminado"<<std::endl;
        optimizer.step();
        // std::cout<<"optimizador"<<std::endl;
    }
}

// Parcialmente completo.
template <class search_type, class Encoder>
void train_model(std::string ModelPath,
                 std::string DataPath,
                 BoardGame& G,
                 int games,
                 int Batch_size,
                 int Num_epoch,
                 nn_utils::neural_options& options_)
{

    Network_evaluator Model(options_);
    Network_evaluator Model_tmp(options_);

    nn_utils::load_net<Network_evaluator>(ModelPath, Model, options_);
    nn_utils::load_net<Network_evaluator>(ModelPath, Model_tmp, options_);
    auto device = nn_utils::get_device();

    Model.to(device);
    Model_tmp.to(device);
    Model_tmp.eval();

    Encoder Encoder_(G.Board);
    torch::optim::SGD optimizer(Model.parameters(),
                                torch::optim::SGDOptions(0.01).momentum(0.5));

    // Condición de paro. (Falta por hacer!)
    int valor_ini = 2;
    while (valor_ini)
    {
        std::cout << "Generando juegos." << std::endl;
        generate_games<search_type, Encoder>(/*Path_to_save = */ DataPath,
                                             /*total_records = */ games,
                                             /*Model = */ Model_tmp,
                                             /*BoardGame = */ G,
                                             /*Encoder*/ Encoder_);

        std::cout << "Creando conjunto de datos." << std::endl;
        auto data = get_data_games<Encoder>(/*Path_to_load=*/DataPath,
                                            /*Encoder= */ Encoder_); // This
                                                                     // return a
                                                                     // torch
                                                                     // example
                                                                     // vector.
        auto Dataset = GameDataSet(data).map(
          torch::data::transforms::Stack<>()); // Transform the dataset in
                                               // understable form for torch.

        torch::data::samplers::RandomSampler sampler(data.size()); // This say
                                                                   // how will
                                                                   // be getting
                                                                   // the data.

        auto DataLoader =
          torch::data::make_data_loader(Dataset, sampler, Batch_size);

        std::cout << "Training net by " << Num_epoch << " epochs." << std::endl;
        for (int epoch_ = 1; epoch_ <= Num_epoch; ++epoch_)
            train_one_epoch(Model, *DataLoader, optimizer);

        search_type current(G, Model, Encoder_, 60, 'B');
        search_type last(G, Model_tmp, Encoder_, 60, 'W');
        std::cout << "Compare progress." << std::endl;
        if (evaluate_accuracy<search_type, search_type>(current, last, G, 30) >=
            .6)
        {
            std::cout << "Obtuvo exito, ha mejorado el modelo" << std::endl;
            nn_utils::save_net<Network_evaluator>(ModelPath, Model);
            nn_utils::load_net<Network_evaluator>(ModelPath, Model_tmp, options_);
            Model_tmp.to(device);
            Model_tmp.eval();
            valor_ini--;
        }
    }
}