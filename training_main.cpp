#include "Include/BoardGame.hpp"
#include "Include/Extra/External/CLI11.hpp"
#include "Include/Extra/Graph.hpp"
#include "Include/Extra/Utilities.hpp"
#include "Include/Extra/json_manage.hpp"
//#include "Search_Algorithms/C_49/Net_trainer.hpp"
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



int main(int argc, char** argv)
{
/*
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

        std::tuple<Graph, std::vector<sf::Vector2i>> graph_ =
          get_json_to_graph_GUI<sf::Vector2i>(GraphFile);
        Graph G = std::get<0>(graph_);
        BoardGame BG(G);

        train_model<SimpleEncoder>(ModelPath, DataPath, BG, Num_games, batch, epoch, device);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

*/

  return 0;
}