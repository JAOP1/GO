#include "Include/BoardGame.hpp"
#include "Include/Extra/External/CLI11.hpp"
#include "Include/Extra/Graph.hpp"
#include "Include/Extra/Utilities.hpp"
#include "Include/Extra/json_manage.hpp"
#include "Search_Algorithms/C_49/Grid/NN_classes/Net_ClassGrid.hpp"
#include "Search_Algorithms/C_49/Grid/Net_trainer.hpp"
#include "Search_Algorithms/C_49/Grid/Include/torch_utils.hpp"
#include "Search_Algorithms/C_49/UCT_Net.hpp"
#include <torch/torch.h>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
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
    CLI11_PARSE(app, argc, argv);

  /*
  ----------------------------------------------------------
  ¡Warning!
  This only works in grids....
  ----------------------------------------------------------

  */

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
        nn_utils::neural_options options_({{3, 5, 3}, {5, 5, 3}, {5, 5, 2}},
                                          {{180, 90}, {90, 45}, {45, 1}});
        GridEncoder encoder_(BG.Board, 5, 5); //Esto es caso especifico, corregir!

        train_model<MCTS_Net<GridNetwork, GridEncoder>, GridEncoder, GridNetwork>(
          /*Model_Path=*/ModelPath,
          /*Data_path=*/DataPath,
          /*GameRuleSet=*/BG,
          /*Encoder=*/encoder_,
          /*Total_games=*/Num_games,
          /*Batch_size=*/batch,
          /*EpochNum=*/epoch,
          /*Net_settings=*/options_);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what();
    }

    
    return 0;
}