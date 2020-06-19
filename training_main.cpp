#include <torch/torch.h>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "Include/Extra/External/CLI11.hpp"
#include "Include/Extra/Graph.hpp"
#include "Include/Extra/Utilities.hpp"
#include "Include/Extra/json_manage.hpp"
#include "Include/BoardGame.hpp"
#include "Search_Algorithms/C_49/UCT_Net.hpp"
#include "Search_Algorithms/Standard_Engines/random_agent.hpp"
#include "Search_Algorithms/C_49/NN_classes/Net_Class.hpp"
#include "Search_Algorithms/C_49/NN_classes/Net_ClassGrid.hpp"
#include "Search_Algorithms/C_49/Net_trainer.hpp"
#include "Search_Algorithms/C_49/torch_utils.hpp"

template <class T>
std::ostream& operator<<(std::ostream& os, std::vector<T>& array)
{
    for (auto i : array)
        os << i << " ";

    return os;
}

struct graph_position
{
    graph_position(int a, int b) : x(a), y(b) {}
    int x;
    int y;
};

int main(int argc, char** argv)
{

    CLI::App app{"---- Go Trainer ----"};

    std::string GraphFile = ""; // Train algorithm for this graph.
    std::string ModelName; // Load neural model.
    std::string selection_mode = "UCT";
    int batch = 2; // Parameters for trainer.
    int epoch = 1;
    int Num_games = 3; // Number of generated games.

    app.add_option("-i", GraphFile, "Load Graph to train.(needed)");
    app.add_option("-b", batch, "Batch size.");
    app.add_option("-e", epoch, "Epoch number.");
    app.add_option("-N", Num_games, "Game records.");
    CLI11_PARSE(app, argc, argv);

    try
    {
        std::vector<std::string> v;
        split(GraphFile, '.', v);

        std::tuple<Graph, std::vector<graph_position>> graph_ =
          json_utils::get_json_to_graph_GUI<graph_position>(GraphFile);

        Graph G = std::get<0>(graph_);
        BoardGame BG(G);

        std::string directory_path = std::filesystem::current_path();
        std::string ModelPath = directory_path +
          "/../Search_Algorithms/C_49/Models/Model_" + v[0] + ".pt";

        std::string DataPath = directory_path +
          "/../Search_Algorithms/C_49/Data/Data_" + v[0] + ".json";

        std::cout << "Model saved in: " << ModelPath << std::endl;
        std::cout << "Data saved in: " << DataPath << std::endl;

        // TODO: Crear un archivo para todas las configuraciones de arquitectura.
        nn_utils::neural_options options_({{3,5,3} , {5,5,3}, {5,5,2}},{{180,90},{90,45},{45,1}});
        GridEncoder2d encoder_(BG.Board,5,5);

        train_model<MCTS_Net<GridNetwork, GridEncoder2d>,
                    GridEncoder2d,GridNetwork>(/*Model_Path=*/ModelPath,
                                      /*Data_path=*/DataPath,
                                      /*GameRuleSet=*/BG,
                                      /*Encoder=*/ encoder_,
                                      /*Total_games=*/Num_games,
                                      /*Batch_size=*/batch,
                                      /*EpochNum=*/epoch,
                                      /*Net_settings=*/options_);
       
    }
    catch (const std::exception& e)
    {
        std::cout << "Invalid graph file." << std::endl;
        std::cerr << e.what();
    }


    /*
    //Comparar nuestro algoritmos contra el azar...
        Network_evaluator NN(options_);
        load_net(ModelPath , NN, options_);
        SimpleEncoder1d encoder_(G);
        MCTS_Net<Network_evaluator,SimpleEncoder1d> Agente1(BG,NN,encoder_,120,'B');
        Agente1.train();
        Random_agent aleatorio;
        auto resultado = evaluate_accuracy<MCTS_Net<Network_evaluator,SimpleEncoder1d> , Random_agent>(Agente1 , aleatorio, BG, 30);
        std::cout<<"porcentaje de ganado: "<<resultado<<std::endl;
    */    

//   //Código para probar red neuronal para grafos en malla... (en lo mientras)
//   int rows = 4;
//   int columns = 4;
//   Graph G = graphs::Grid(rows, columns);
//   BoardGame BG(G);
//   game D;
//   GridEncoder2d encoder(G,/*rows=*/5,/*cols=*/5);

//   std::vector<vertex> actions = BG.get_available_sample_cells(1.0);
//   D.add_game_state({0.25, 0.25, 0.25, 0.25}, BG.show_current_state(),
// actions);

//   BG.make_action(0);
//   actions = BG.get_available_sample_cells(1.0);
//   D.add_game_state({0, 0.333, 0.333, 0.333}, BG.show_current_state(),
// actions);

//   BG.make_action(1);
//   actions = BG.get_available_sample_cells(1.0);
//   D.add_game_state({0, 0, 0.5, 0.5}, BG.show_current_state(), actions);

//   D.set_reward(BG.reward('B'));

//   BG.make_action(15);
//   actions = BG.get_available_sample_cells(1.0);
//   D.add_game_state({0,0,0,0} , BG.show_current_state(),actions);

//   auto vector_torch_planes = encoder.Encode_episode<game>(D);

//   // std::cout<<vector_torch_planes.size()<<std::endl;
//   // std::cout<<vector_torch_planes[3].data<<std::endl;
//   // std::cout<<vector_torch_planes[3].target<<std::endl;

//   //Probando la arquitectura de la red...
//   auto Dataset = GameDataSet(vector_torch_planes).map(
//           torch::data::transforms::Stack<>()); // Transform the dataset in
//                                                // understable form for torch.

//   torch::data::samplers::RandomSampler sampler(vector_torch_planes.size()); // This say
//                                                               // how will
//                                                               // be getting
//                                                               // the data.

//   auto DataLoader =
//     torch::data::make_data_loader(Dataset, sampler, 2);

//   nn_utils::neural_options options_({{3,5,3} , {5,5,3}, {5,5,2}},{{180,90},{90,45},{45,1}});
//   GridNetwork NN(options_);
//   auto device = nn_utils::get_device();
//   NN.to(device);
//   NN.eval();

//   for(auto& batch : *DataLoader )
//   {
//     auto data = batch.data.to(device);
//     std::cout<<"Tamanio de datos: "<<data.sizes()<<std::endl;
//     auto output  = NN.forward(data);
//     //std::cout<<output<<std::endl;
//   }

  return 0;
}