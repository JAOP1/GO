#include "Include/BoardGame.hpp"
#include "Include/Extra/External/CLI11.hpp"
#include "Include/Extra/Graph.hpp"
#include "Include/Extra/Utilities.hpp"
#include "Include/Extra/json_manage.hpp"
//Incluir estas lineas.
#include "Search_Algorithms/C_49/RAVE_Net.hpp"
#include "Search_Algorithms/C_49/UCT_Net.hpp"
#include "Search_Algorithms/C_49/Net_trainer.hpp"
#include "Search_Algorithms/C_49/game_structure.hpp"
//#include "Search_Algorithms/C_49/encoders.hpp"
#include "Search_Algorithms/C_49/Net_Class.hpp"
//#include "Search_Algorithms/C_49/Data_manage.hpp"
#include <torch/torch.h>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

template<class T>
std::ostream& operator<<(std::ostream& os , std::vector<T>& array)
{
  for(auto i : array)
    os<<i<<" ";

  return os;
}


struct graph_position
{
  graph_position(int a, int b):x(a) , y(b){}
  int x;
  int y;
};


using element = torch::data::Example<torch::Tensor, torch::Tensor>;

int main(int argc, char** argv)
{

    // Esto es para proobar Ggg.
    CLI::App app{"---- Go Trainer ----"};

    std::string GraphFile = ""; // Train algorithm for this graph.
    std::string ModelName; // Load neural model.
    std::string selection_mode = "UCT";
    int batch = 80; // Parameters for trainer.
    int epoch = 10;
    int Num_games = 100; // Number of generated games.

    app.add_option("-i", GraphFile, "Load Graph to train.(needed)");
    app.add_option("-b", batch, "Batch size.");
    app.add_option("-e", epoch, "Epoch number.");
    app.add_option("-N", Num_games, "Game records.");
    app.add_option("-p", selection_mode, "type name (default = UCT): RAVE");
    CLI11_PARSE(app, argc, argv);

    torch::DeviceType device_type;
    if (torch::cuda::is_available())
    {
        std::cout << "CUDA available! Training on GPU." << std::endl;
        std::cout << "Cuda diveces:" << torch::cuda::device_count() << std::endl;
        device_type = torch::kCUDA;
    }
    else
    {
        std::cout << "Training on CPU." << std::endl;
        device_type = torch::kCPU;
    }

    torch::Device device(device_type);

    try
    {
        std::vector<std::string> v;
        split(GraphFile, '.', v);

        std::string directory_path = std::filesystem::current_path();
        std::string ModelPath = directory_path +
          "../Search_Algorithms/C_49/Models/Model_" + v[0] + ".pt";

        std::string DataPath = directory_path +
          "../Search_Algorithms/C_49/Data/Data_" + v[0] + ".json";

        std::tuple<Graph, std::vector<graph_position>> graph_ =
          get_json_to_graph_GUI<graph_position>(GraphFile);
        Graph G = std::get<0>(graph_);
        BoardGame BG(G);

        if(selection_mode == "UCT")
          train_model<MCTS_Net<Network_evaluator,SimpleEncoder1d >,SimpleEncoder1d>(ModelPath, DataPath, BG, Num_games, batch, epoch, device);
        else if(selection_mode == "RAVE")
          train_model<MC_RAVE<Network_evaluator,SimpleEncoder1d>,SimpleEncoder1d>(ModelPath, DataPath, BG, Num_games, batch, epoch, device);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }



/*
  //Todo esto fue para probar!! 
  int rows = 4;
  int columns = 4;
  Graph G = graphs::Grid(rows, columns);
  BoardGame BG(G);
  game D;
  SimpleEncoder2d encoder2d(G);
  SimpleEncoder1d encoder1d(G);

  std::vector<vertex> actions = BG.get_available_sample_cells(1.0);
  D.add_game_state({0.25, 0.25, 0.25, 0.25}, BG.show_current_state(), actions);

  BG.make_action(0);
  actions = BG.get_available_sample_cells(1.0);
  D.add_game_state({0, 0.333, 0.333, 0.333}, BG.show_current_state(), actions);

  BG.make_action(1);
  actions = BG.get_available_sample_cells(1.0);
  D.add_game_state({0, 0, 0.5, 0.5}, BG.show_current_state(), actions);

  D.set_reward(BG.reward('B'));

  auto vector_torch_planes_2d = encoder2d.Encode_episode<game>(D);
  auto vector_torch_planes_1d = encoder1d.Encode_episode<game>(D);

  
  std::cout<<"Resultado con encoder 2d: "<<std::endl;
  std::cout<<vector_torch_planes_2d[1].data<<std::endl;
  std::cout<<vector_torch_planes_2d[1].target<<std::endl;
  
  std::cout<<"Resultado con encoder 1d:"<<std::endl;
  std::cout<<vector_torch_planes_1d[1].data<<std::endl;
  std::cout<<vector_torch_planes_1d[1].target<<std::endl;

  
  torch::data::samplers::RandomSampler sampler(vector_torch_planes_1d.size());
  auto  F = GameDataSet(vector_torch_planes_1d).map(torch::data::transforms::Stack<>());
  auto data_loader = torch::data::make_data_loader(F, sampler, 2);

  std::cout<<"He llegado a la parte de la red neuronal."<<std::endl;
  Network_evaluator NN(4);
  NN.to(torch::kCUDA);
  NN.eval();

  for (const auto& batch : *data_loader) {
    auto data = batch.data.to(torch::kCUDA), targets = batch.target.to(torch::kCUDA);
    
    //std::cout<<data.sizes()<<std::endl;
    //std::cout<<targets.sizes()<<std::endl;
    auto output = NN.forward(data);
    std::cout<<output.sizes()<<std::endl;
    std::cout<<"----"<<std::endl;
  }
*/

  return 0;
}