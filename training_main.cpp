#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <exception>
#include <memory>
#include <torch/torch.h>
#include "Include/Extra/Graph.hpp"
#include "Include/Extra/json_manage.hpp"
#include "Include/Extra/Utilities.hpp"
#include "Include/BoardGame.hpp"
#include "Include/Extra/External/CLI11.hpp"
#include "Search_Algorithms/C_49/encoders.hpp"
#include "Search_Algorithms/C_49/loading_data.hpp"
#include "Search_Algorithms/C_49/Net_Class.hpp"

size_t dataset_size;





template <typename DataLoader>
void train_one_epoch(Network_evaluator& model , DataLoader& loader , torch::Device device , torch::optim::Optimizer& optimizer )
{

    model.train();
    size_t batch_idx = 0;
    for (auto& batch : data_loader) {
        auto data = batch.data.to(device);
        auto targets = batch.target.to(device);
        optimizer.zero_grad();
        torch::Tensor output = model.forward(data);
        //Aqui modificar.
        auto loss = torch::nll_loss(output, targets);
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
    Network_evaluetor Model;
    //Si quieres continuar entrenado un modelo.
    if(std::filesystem::exists( ModelName))
    {
        auto load_model_ptr = std::make_shared<Network_evaluator>();
        torch::load( load_model_ptr, ModelName);
        Model = *load_model_ptr;
    }

    Model.to(device);
    
    SimpleEncoder Enconder_(G) ; //Voy a pensar como hacer esto...
    torch::optim::Adam optimizer(Model.parameters());
    while(){
        generate_games(/*Path_to_save = */ DataPath, /*total_records = */ games , /*Model = */Model ,/*BoardGame = */ G );    
        auto DataSet = get_data_games(/*Path_to_load=*/DataPath, /*Encoder= */ Encoder_ ); // This return the Datastructure.
        torch::data::samplers::RandomSampler sampler(DataSet.get_size());
        auto DataLoader = torch::data::make_data_loader(DataSet, sampler, Batch_size);
        dataset_size = DataSet.size();

        for(size_t epoch_ = 1 ; epoch_ <= Num_epoch ; ++epoch_ )
            train_one_epoch(Model , *DataLoader , device , optimizer);    
        
        if()
        {
            auto Net = std::make_shared<Net>(Model);
            torch::save(Net, ModelName); 
        }
    }

    //Save model here.

}



int main(int argc, char** argv)
{

    // Esto es para proobar Ggg.
    CLI::App app{"---- Go Trainer ----"};


    std::string GraphFile = ""; //Train algorithm for this graph.
    std::string ModelName; //Load neural model.
    int batch = 80;  // Parameters for trainer.
    int epoch = 10;
    int Num_games = 100; //Number of generated games. 

    app.add_option("-i", GraphFile, "Load Graph to train.(needed)");
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
        std::vector<std::string> v;
        split(GraphFile, '.', v);
        

        std::string directory_path = std::filesystem::current_path();
        std::string ModelPath = directory_path+"../Search_Algorithms/C_49/Models/Model_" + v[0] + ".pt";

        std::string DataPath = directory_path+"../Search_Algorithms/C_49/Data/Data_"+v[0]+".json";

        std::tuple<Graph, std::vector<sf::Vector2i>> graph_ = get_json_to_graph_GUI<sf::Vector2i>(GraphFile);   
        Graph G = std::get<0>(graph_);
        BoardGame BG(G);

        train_model(ModelPath,Num_games , BG , batch , epoch, device , DataPath);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}