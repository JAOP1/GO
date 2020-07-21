#pragma once
#include "../../Include/Extra/Utilities.hpp"
#include "Include/Data_manage.hpp"
#include "Include/torch_utils.hpp"
#include <torch/torch.h>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <string>

size_t dataset_size;

template <typename loss>
loss calculate_loss(torch::Tensor& data, torch::Tensor target)
{
    // It only works in policy and value network.
    // int policy_vector_leght = target.sizes()[1];
    // torch::Tensor policy_target = target.index(
    //   {Slice(), Slice(None, policy_vector_leght - 1)});
    // torch::Tensor value_target = target.index({Slice(), policy_vector_leght -
    // 1}); torch::Tensor policy_data = data.index(
    //   {Slice(), Slice(None, policy_vector_leght - 1)});
    // torch::Tensor value_data = data.index({Slice(), policy_vector_leght - 1});

    // auto output1 = torch::nn::functional::binary_cross_entropy(policy_data,
    //                                                            policy_target);
    // auto output2 = torch::nn::functional::mse_loss(value_data, value_target);
    // return output1 + output2;
    auto output = torch::nn::functional::mse_loss(data, target);
    return output;
}

template <typename DataLoader, class NN>
void train_one_epoch(NN& model,
                     DataLoader& loader,
                     torch::optim::Optimizer& optimizer)
{
    std::cout << "Training one epoch." << std::endl;
    auto device = nn_utils::get_device();

    model.train();
    double test_loss = 0;
    // size_t batch_idx = 0;
    for (auto& batch : loader)
    {
        auto data = batch.data.to(device);
        auto targets = batch.target.to(device);
        optimizer.zero_grad();
        torch::Tensor output = model.forward(data);
        torch::Tensor loss = calculate_loss<torch::Tensor>(output, targets);
        loss.backward();
        optimizer.step();

        test_loss += loss.item<double>();
    }

    test_loss /= dataset_size;
    std::cout << "Average loss= " << test_loss << std::endl;
}

// Parcialmente completo.
template <class search_type, class Encoder, class NN>
void train_model(std::string ModelPath,
                 std::string DataPath,
                 BoardGame& G,
                 Encoder& Encoder_,
                 int games,
                 int Batch_size,
                 int Num_epoch,
                 nn_utils::neural_options& options_)
{

    NN Model(options_);
    NN Model_tmp(options_);

    nn_utils::load_net<NN>(ModelPath, Model, options_);
    nn_utils::load_net<NN>(ModelPath, Model_tmp, options_);
    auto device = nn_utils::get_device();

    Model.to(device);
    Model_tmp.to(device);
    Model_tmp.eval();

    //Seleccionar el mejor optimizador....
    // torch::optim::SGD optimizer(Model.parameters(),
    //                             torch::optim::SGDOptions(0.01).momentum(0.5));
    torch::optim::Adam optimizer(Model.parameters(),
                                 torch::optim::AdamOptions(0.001));

    // Condición de paro. (Falta por hacer!)
    //Actualmente se detiene cuando mejora el modelo 2 veces.
    int valor_ini = 2;
    while (valor_ini)
    {
        std::cout << "Generando juegos." << std::endl;
        generate_games<search_type, Encoder, NN>(/*Path_to_save = */ DataPath,
                                                 /*total_records = */ games,
                                                 /*Model = */ Model_tmp,
                                                 /*BoardGame = */ G,
                                                 /*Encoder*/ Encoder_);

        std::cout << "Creando conjunto de datos." << std::endl;
        auto data = get_data_games<Encoder>(/*Path_to_load=*/DataPath,
                                            /*Encoder= */ Encoder_); 
        dataset_size = data.size();
        std::cout << "Total de datos: " << dataset_size << std::endl;
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

        std::cout << "Compare progress." << std::endl;
        search_type current(G, Model, Encoder_, 60, 'B');
        search_type last(G, Model_tmp, Encoder_, 60, 'W');
        current.eval(); //Evaluation mode.
        last.eval(); //Evaluation mode.
        //If the model win more than 55%, it means that model has improved.
        if (evaluate_accuracy<search_type, search_type>(current, last, G, 30) >=
            .55)
        {
            std::cout << "Obtuvo exito, ha mejorado el modelo" << std::endl;
            nn_utils::save_net<NN>(ModelPath, Model);
            nn_utils::load_net<NN>(ModelPath, Model_tmp, options_);
            Model_tmp.to(device);
            Model_tmp.eval();
            valor_ini--; // If the model improve twice, we stop...

            // Save when model improve.
            std::ofstream changelog;
            std::string dir_path = std::filesystem::current_path();
            std::string file_path = "/../Search_Algorithms/C_49/changelog.txt";
            changelog.open(dir_path + file_path, std::ios::app);
            time_t now = std::time(0);
            char* dt = std::ctime(&now);
            changelog << "Model improved: " << dt;
            changelog.close();
        }
    }
}