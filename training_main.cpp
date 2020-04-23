#include <iostream>
#include <vector>
#include "Include/Extra/json_manage.hpp"
#include "CLI11.hpp"

int main(int argc, char** argv)
{

    // Esto es para proobar Ggg.
    CLI::App app{"---- Go Trainer ----"};


    std::string GraphFile = ""; //Train algorithm for this graph.
    std::string ModelName = ""; //Load neural model.
    std::vector<int> NNParams; // Parameters for trainer.
    int Num_games; //Number of generated games. 

    app.add_option("-n",ModelName, "Create or load neural model (if exist).");
    app.add_option("-i", GraphFile, "Load Graph to train.");
    app.add_option("--vec" , NNParams , "Batch size and epoch.");
    app.add_option("-N" , "--Num" , Num_games , "Game records.");

    CLI11_PARSE(app, argc, argv);



    torch::data::samplers::RandomSampler sampler(X.size());
    GameDateSet G(X,Y);
    auto data_loader = torch::data::make_data_loader(G, sampler, batch_size);


    return 0;
}