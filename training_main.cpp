#include "Include/BoardGame.hpp"
#include "Include/Extra/External/CLI11.hpp"
#include "Include/Extra/Graph.hpp"
#include "Include/Extra/Utilities.hpp"
#include "Include/Extra/json_manage.hpp"
// Incluir estas lineas.
//#include "Search_Algorithms/C_49/RAVE_Net.hpp"
//#include "Search_Algorithms/C_49/UCT_Net.hpp"
//#include "Search_Algorithms/C_49/game_structure.hpp"
//#include "Search_Algorithms/C_49/Net_Class.hpp"
#include "Search_Algorithms/C_49/Net_trainer.hpp"
#include "Search_Algorithms/C_49/torch_utils.hpp"
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <torch/torch.h>
#include <vector>

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
    app.add_option("-p", selection_mode, "type name (default = UCT): RAVE");
    CLI11_PARSE(app, argc, argv);

    try
    {
        std::vector<std::string> v;
        split(GraphFile, '.', v);

        std::tuple<Graph, std::vector<graph_position>> graph_ =
          get_json_to_graph_GUI<graph_position>(GraphFile);

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
        nn_utils::neural_options options_({{3, 5, 3}, {5, 10, 3}},
                                          {{6210, 1540}, {1540, 360}, {360, 27}});
        // torch::autograd::AnomalyMode::set_enabled(true); //NO sé como usar...
        if (selection_mode == "UCT")
            train_model<MCTS_Net<Network_evaluator, SimpleEncoder1d>,
                        SimpleEncoder1d>(/*Model_Path=*/ModelPath,
                                         /*Data_path=*/DataPath,
                                         /*GameRuleSet=*/BG,
                                         Num_games,
                                         /*Batch_size=*/batch,
                                         /*EpochNum=*/epoch,
                                         options_);
        // else if(selection_mode == "RAVE")
        //  train_model<MC_RAVE<Network_evaluator,SimpleEncoder1d>,SimpleEncoder1d>(ModelPath,
        //  DataPath, BG, Num_games, batch, epoch, options_);
    }
    catch (const std::exception& e)
    {
        std::cout << "Invalid graph file." << std::endl;
        std::cerr << e.what();
    }

    return 0;
}