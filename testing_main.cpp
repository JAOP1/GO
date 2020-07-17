#include <iostream>
#include <vector>
#include "Include/Extra/External/CLI11.hpp"
#include "Include/Extra/json_manage.hpp"
#include "Include/Extra/Graph.hpp"
#include "Include/Extra/Utilities.hpp"
#include "Search_Algorithms/testing/random_samples.hpp"

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

    switch(block_to_test)
    {
        case 0:
            generate_random_samples(/*num_games=*/1000 ,G, DataPath);
            break;
        default:
            std::cout<<"No se ha corrido ninguna prueba"<<std::endl;
            break;
    }
}