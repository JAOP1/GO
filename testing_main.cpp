#include <iostream>
#include <vector>
#include "Include/Extra/External/CLI11.hpp"
#include "Include/Extra/json_manage.hpp"
#include "Include/Extra/Graph.hpp"
#include "Include/Extra/Utilities.hpp"

#include "Search_Algorithms/C_49/Include/torch_utils.hpp"
#include "Search_Algorithms/C_49/NN_classes/Net_ClassGrid.hpp"
#include "Search_Algorithms/C_49/Encoders_classes/encoders.hpp"

#include "Search_Algorithms/testing/random_samples.hpp"
#include "Search_Algorithms/testing/network_train.hpp"


struct graph_position
{
    graph_position(int a, int b) : x(a), y(b) {}
    int x;
    int y;
};


int main(int argc, char** argv)
{


    CLI::App app{"Ejecutar pruebas..."};

    std::string GraphFile = ""; // Train algorithm for this graph.
    int block_to_test = -1;


    app.add_option("-i", GraphFile, "Load Graph to train.(needed)");
    app.add_option("-n", block_to_test, "number test ");
    CLI11_PARSE(app, argc, argv);

    std::vector<std::string> v;
    split(GraphFile, '.', v);

    std::tuple<Graph, std::vector<graph_position>> graph_ =
    json_utils::get_json_to_graph_GUI<graph_position>(GraphFile);

    Graph G = std::get<0>(graph_);

    std::string directory_path = std::filesystem::current_path();
    std::string DataPath = directory_path +
        "/../Search_Algorithms/testing/Data_" + v[0] + ".json";
    std::string ModelPath = directory_path +
          "/../Search_Algorithms/testing/Model_" + v[0] + ".pt";

    //Esto es necesario para la segunda opción.
    nn_utils::neural_options options_({{3, 5, 3}, {5, 5, 3}, {5, 5, 2}},
                                     {{180, 90}, {90, 45}, {45, 1}});
    GridNetwork Model(options_); 
    nn_utils::load_net<GridNetwork>(ModelPath, Model, options_);
    GridEncoder2d Encoder_(G, 5, 5);

    switch(block_to_test)
    {
        case 0:
            generate_random_samples(/*num_games=*/1000 ,G, DataPath);
            break;
        case 1:
            for_train<GridNetwork,GridEncoder2d>(Model, Encoder_, DataPath, 10);
            break;
        default:
            std::cout<<"No se ha corrido ninguna prueba"<<std::endl;
            break;
    }
}