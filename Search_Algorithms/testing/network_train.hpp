#pragma once 
#include "../../Include/Extra/Graph.hpp"
#include "../C_49/Grid/Net_trainer.hpp"
#include "../C_49/Grid/Include/Data_manage.hpp"
#include "../C_49/Grid/Include/torch_utils.hpp"
#include "../C_49/Grid/NN_classes/Net_ClassGrid.hpp"
#include "../C_49/Grid/Encoders_classes/encoders.hpp"
#include <torch/torch.h>


/* 
    Todo esto fue desarrollado para probar grid5.json
    para ejecutar en consola este test es:
    ./tester -i grid5.json -n 1
*/
void for_train(  std::string DataPath,  std::string ModelPath, const Graph& G, int n)
{
    nn_utils::neural_options options_({{3, 5, 3}, {5, 5, 3}, {5, 5, 2}},
                                     {{180, 90}, {90, 45}, {45, 1}});
    GridNetwork Model(options_); 
    nn_utils::load_net<GridNetwork>(ModelPath, Model, options_);
    Model.to(nn_utils::get_device());
    GridEncoder Encoder_(G, 5, 5);

    torch::optim::Adam optimizer(Model.parameters(),
                                 torch::optim::AdamOptions(0.001));

    auto data = game_utils::get_data_games<GridEncoder>(/*Path_to_load=*/DataPath,
                                            /*Encoder= */ Encoder_); 

    auto Dataset = GameDataSet(data).map(
        torch::data::transforms::Stack<>()); // Transform the dataset in
                                            // understable form for torch.
    torch::data::samplers::RandomSampler sampler(data.size()); // This say
                                                                // how will
                                                                // be getting
                                                                // the data.
    auto DataLoader =
        torch::data::make_data_loader(Dataset, sampler, 110);
    for(int i = 0;  i < n ; ++i)
    {
        train_one_epoch(Model, *DataLoader, optimizer, data.size());
    }
}