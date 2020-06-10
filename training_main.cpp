#include "Include/BoardGame.hpp"
#include "Include/Extra/External/CLI11.hpp"
#include "Include/Extra/Graph.hpp"
#include "Include/Extra/Utilities.hpp"
#include "Include/Extra/json_manage.hpp"
//Incluir estas lineas.
//#include "Search_Algorithms/C_49/RAVE_Net.hpp"
//#include "Search_Algorithms/C_49/UCT_Net.hpp"
//#include "Search_Algorithms/C_49/game_structure.hpp"
//#include "Search_Algorithms/C_49/Net_Class.hpp"
#include "Search_Algorithms/C_49/Net_trainer.hpp"
#include "Search_Algorithms/C_49/torch_utils.hpp"
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
        std::cout<<"Model saved in: "<<ModelPath<<std::endl;

        std::string DataPath = directory_path +
          "/../Search_Algorithms/C_49/Data/Data_" + v[0] + ".json";

        std::cout<<"Data saved in: "<<DataPath<<std::endl;

        //TODO: Crear un archivo para todas las configuraciones de arquitectura.
        nn_utils::neural_options options_({{3,5,3}, {5,10,3}} ,
                          {{6210,1540},{1540,360},{360,27}});

        if(selection_mode == "UCT")
          train_model<MCTS_Net<Network_evaluator,SimpleEncoder1d >,SimpleEncoder1d>(/*Model_Path=*/ModelPath,/*Data_path=*/ DataPath,/*GameRuleSet=*/ BG, Num_games,/*Batch_size=*/ batch,/*EpochNum=*/ epoch , options_);
        //else if(selection_mode == "RAVE")
        //  train_model<MC_RAVE<Network_evaluator,SimpleEncoder1d>,SimpleEncoder1d>(ModelPath, DataPath, BG, Num_games, batch, epoch, options_);
    }
    catch (const std::exception& e)
    {
        std::cout <<"Invalid graph file."<< std::endl;
        std::cerr<<e.what();
    }




//   //Todo esto fue para probar!! 
//   int rows = 4;
//   int columns = 4;
//   Graph G = graphs::Grid(rows, columns);
//   BoardGame BG(G);
//   game D;
//   SimpleEncoder2d encoder2d(G);
//   SimpleEncoder1d encoder1d(G);

//   std::vector<vertex> actions = BG.get_available_sample_cells(1.0);
//   D.add_game_state({0.25, 0.25, 0.25, 0.25}, BG.show_current_state(), actions);

//   BG.make_action(0);
//   actions = BG.get_available_sample_cells(1.0);
//   D.add_game_state({0, 0.333, 0.333, 0.333}, BG.show_current_state(), actions);

//   BG.make_action(1);
//   actions = BG.get_available_sample_cells(1.0);
//   D.add_game_state({0, 0, 0.5, 0.5}, BG.show_current_state(), actions);

//   D.set_reward(BG.reward('B'));

//   auto vector_torch_planes_2d = encoder2d.Encode_episode<game>(D);
//   auto vector_torch_planes_1d = encoder1d.Encode_episode<game>(D);

//   std::cout<<"Encoder funciona bien!"<<std::endl;
  
//   //std::cout<<"Resultado con encoder 2d: "<<std::endl;
//   //std::cout<<vector_torch_planes_2d[1].data<<std::endl;
//   //std::cout<<vector_torch_planes_2d[1].target<<std::endl;
  
//   //std::cout<<"Resultado con encoder 1d:"<<std::endl;
//   //std::cout<<vector_torch_planes_1d[1].data<<std::endl;
//   //std::cout<<vector_torch_planes_1d[1].target<<std::endl;


//  neural_options options_({{3,5,3}, {5,10,3}} ,
//                          {{6210,1540},{1540,360},{360,27}});

//   //Network_evaluator NN("Search_Algorithms/C_49/options_net.txt");
//   Network_evaluator NN(options_);
//   auto device = get_device(/*verbose=*/true);
//   NN.to(device);
//   NN.eval();

//   std::string path = "prueba_net.pt";
//   std::cout<<"Salvando modelo..."<<std::endl;
//   save_net<Network_evaluator>(path, NN);
//   std::cout<<NN<<std::endl;
//   std::cout<<"Red neuronal creada!"<<std::endl;

//   // MCTS_Net<Network_evaluator , SimpleEncoder1d> agente(BG,NN,encoder1d,323, 'B');
//   // std::cout<<agente.search(BG)<<std::endl;
//   // std::cout<<agente.get_probabilities_current_state()<<std::endl;
//   // torch::Tensor input = encoder1d.Encode_data(D.states[0], D.valid_actions[0], 0,true);
  
//   // auto input_cud = input.to(device);
 
//   //  auto resultado = NN.forward(input_cud);
//   //  std::cout<<resultado<<std::endl;
//   //  std::cout<<resultado[0][26].item<double>()<<std::endl;
 


//   torch::data::samplers::RandomSampler sampler(vector_torch_planes_1d.size());
//   auto  F = GameDataSet(vector_torch_planes_1d).map(torch::data::transforms::Stack<>());
//   auto data_loader = torch::data::make_data_loader(F, sampler, 2);

//   for (const auto& batch : *data_loader) {
//     auto data = batch.data.to(device), targets = batch.target.to(device);
    
//     //std::cout<<data.sizes()<<std::endl;
//     //std::cout<<targets.sizes()<<std::endl;
//     auto output = NN.forward(data);
//     std::cout<<calculate_loss<torch::Tensor>(output , targets)<<std::endl;
//     std::cout<<output.sizes()<<std::endl;
//     std::cout<<targets.sizes()<<std::endl;
//     std::cout<<"----"<<std::endl;
//   }


  return 0;
}