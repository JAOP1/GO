#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <exception>
#include <torch/torch.h>
#include "Include/Extra/Graph.hpp"
#include "Include/Extra/json_manage.hpp"
#include "Include/Extra/Utilities.hpp"
#include "Include/BoardGame.hpp"
#include "Include/Extra/External/CLI11.hpp"

size_t dataset_size;

template <typename DataLoader>
void train_one_epoch(Net& model , DataLoader& loader , torch::Device device , torch::optim::Optimizer& optimizer )
{

    model.train();
    size_t batch_idx = 0;
    for (auto& batch : data_loader) {
        auto data = batch.data.to(device), targets = batch.target.to(device);
        optimizer.zero_grad();
        auto output = model.forward(data);
        auto loss = torch::nll_loss(output, targets);
        AT_ASSERT(!std::isnan(loss.template item<float>()));
        loss.backward();
        optimizer.step();

        if (batch_idx++ % 4 == 0) {
        std::printf(
            "\rTrain Epoch: [%5ld/%5ld] Loss: %.4f",
            batch_idx * batch.data.size(0),
            dataset_size,
            loss.template item<float>());
        }
    }

}



void train_model(std::string ModelName , int games , BoardGame& G ,  int  Batch_size, int Num_epoch , torch::Device device , std::string DataPath)
{
    Net Model;
    //Si quieres continuar entrenado un modelo.
    if(std::filesystem::exists( ModelName))
        torch::load(Model , ModelName);

    Model.to(device);
    Model.train();
    
    auto Enconde_ = ; //Voy a pensar como hacer esto...
    torch::optim:: optimizer();
    while(){
        generate_games(/*Path_to_save = */ DataPath, /*total_records=*/ games); //
        auto DataSet = get_data_games(/*Path_to_load=*/DataPath, /*Encoder= */ ); // This will be a datastruct.
        torch::data::samplers::RandomSampler sampler(DataSet.get_size());
        auto DataLoader = torch::data::make_data_loader(G, sampler, Batch_size);
        dataset_size = DataSet.size();

        for(size_t epoch_ = 1 ; epoch_ <= Num_epoch ; ++epoch_ )
            train_one_epoch(Model , *DataLoader , device , optimizer);    
        
        if()
            torch::save(Model , ModelName); //Ahorita pienso como salvar 
    }

    //Save model here.

}



int main(int argc, char** argv)
{

    // Esto es para proobar Ggg.
    CLI::App app{"---- Go Trainer ----"};


    std::string GraphFile = ""; //Train algorithm for this graph.
    std::string ModelName = "Model.pt"; //Load neural model.
    int batch = 80;  // Parameters for trainer.
    int epoch = 10;
    int Num_games = 100; //Number of generated games. 

    app.add_option("-n",ModelName, "Create or load neural model (if exist).");
    app.add_option("-i", GraphFile, "Load Graph to train.");
    app.add_option("--batch" , batch, "Batch size.");
    app.add_option("--epoch" , epoch , "Epoch number.");
    app.add_option("-N" , "--Num" , Num_games , "Game records.");

    CLI11_PARSE(app, argc, argv);

    
    torch::DeviceType device_type;
    if (torch::cuda::is_available()) {
        std::cout << "CUDA available! Training on GPU." << std::endl;
        std::cout<<"Cuda diveces:" << torch::cuda::device_count()<<std::endl;
        device_type = torch::kCUDA;
    } else {
        std::cout << "Training on CPU." << std::endl;
        device_type = torch::kCPU;
    }

    torch::Device device(device_type);




    try
    {

        std::string directory_path = std::filesystem::current_path();
        std::string ModelPath = directory_path+"/Models/" + ModelName;

        std::vector<std::string> v;
        split(ModelName, '.', v);
        std::string DataPath = directory_path+"/Data/"+v[0]+".json";

        std::tuple<Graph, std::vector<sf::Vector2i>> graph_ = get_json_to_graph_GUI<sf::Vector2i>(GraphFile);   
        Graph G = std::get<0>(graph_);
        BoardGame BG(G);
        train_model(ModelName,Num_games , BG , batch , epoch, device , DataPath);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    





    return 0;
}