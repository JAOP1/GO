#include "CLI11.hpp"
#include "GUI/BoardGame.hpp"
#include "GUI/Client.hpp"
#include "GUI/GraphGUI.hpp"
#include "GUI/Include/Extra/Graph.hpp"
#include "GUI/Include/Search_Algorithms/MCTS.hpp"
#include "GUI/Include/Search_Algorithms/MCT_UCT_RAVE.hpp"
#include "GUI/Include/Extra/json_manage.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <tuple>

template <class search_algorithm1, class search_algorithm2>
bool is_winner(search_algorithm1& player1,
               search_algorithm2& player2,
               BoardGame board)
{

    auto first_to_play = board.player_status();
    auto second_to_play = (first_to_play == 'B' ? 'W' : 'B');

    // Para que no realicen la misma acción siempre.
    board.make_action(board.random_action());
    board.make_action(board.random_action());

    vertex action;

    for (int i = 0; i < 60 && !board.is_complete(); ++i)
    {
        auto action_set = board.get_available_sample_cells(1.0);

        // Caso de que no haya una accion para hacer.
        if (action_set.size() == 1 && action_set[0] == -1)
        {
            board.make_action(-1);
            continue;
        }

        if (first_to_play == board.player_status())
            action = player1.search(board);
        else
            action = player2.search(board);

        board.make_action(action);
    }

    return board.reward(second_to_play) < board.reward(first_to_play);
}

template <class search_algorithm1, class search_algorithm2>
double evaluate_accuracy(search_algorithm1& player1,
                         search_algorithm2& player2,
                         BoardGame& board,
                         int games_played)
{
    double win_rate = 0.0;
    std::cout
      << "Evaluamos el número de veces que gana el algoritmo 1 contra el otro."
      << std::endl;
    for (int i = 0; i < games_played; ++i)
    {
        std::cout << "Game number " << i << std::endl;
        if (is_winner(player1, player2, board))
            win_rate++;
    }

    win_rate *= (1.0/games_played);
    return win_rate;
}

//Solución parcial....
void run_search(const std::string& player ,  GUI& client)
{


    if (player == "RAVE")
    {
        MC_RAVE Black_player_RAVE(200, 100, 'B');
        client.Run_VS_AI(Black_player_RAVE, true); // Play against algorithm.
    }
    else if (player == "MCT_UCT"){

        MCTS Black_player(25, 100, 'B');
        client.Run_VS_AI(Black_player, true);
    }
    else
        client.Run(); // Play against someone else.
}

int main(int argc, char** argv)
{
    // Esto es para proobar Ggg.
    CLI::App app{"---- Go generalized ----"};

    // Define options
    bool visualize = true; // If you visualize the board then you aren't
                           // interested in compare algorithms.
    std::string player = "Human";
    std::string output_file = "Graph.json";
    std::string input_file = "";

    app.add_option("-p", player, "type name (default = Human): MCT_UCT, RAVE");
    app.add_option("-o", output_file , "Save Graph visualizer in JSON (default file = Graph_create.json) ");
    app.add_option("-i", input_file , "Load Graph visualizer");
    CLI11_PARSE(app, argc, argv);


    // Si queremos visualizar las acciones y jugar.
    if (visualize)
    {
        if(input_file != "")
        {
            std::tuple<Graph, std::vector<sf::Vector2i>> graph_GUI =  get_json_to_graph_GUI<sf::Vector2i>(input_file);
            std::vector<sf::Vector2i> arr =std::get<1>(graph_GUI);
            Graph G = std::get<0>(graph_GUI);
            
            BoardGame ruleset(G);
            BoardGraphGUI Graph_visualizer(ruleset , arr);

            GUI APP(Graph_visualizer , 700,700,output_file , false);
            run_search(player , APP);
        }
        else
        {
            Graph G(0);
            BoardGame Tablero(G); // Create rules set.
            BoardGraphGUI Graph1(Tablero); // Create graph graphic.
            GUI APP(Graph1, 700, 700, output_file); // Listener and main window loop.
            run_search(player, APP);
        }
        
        
        
    }
    /*
    else
    {
        Graph G = graphs::Grid(3, 3);
        BoardGame Tablero(G);
        auto win_rate_first_algorithm =
          evaluate_accuracy(Black_player_RAVE, White_player, Tablero, 50);

        std::cout << "The first win in a rate of " << win_rate_first_algorithm
                  << std::endl;

        
        //Save result in a extern file.
        std::ofstream myfile;
        myfile.open ("results.txt");
        myfile <<"The MCTS and MCTS2 params: \n"<< "Num simulation: "<< 25
               << "\n Num repetitions: "<<100<<std::endl; myfile <<"Total games
        played: 30"<<std::endl; myfile << "Final result "<<
        win_rate_first_algorithm << " by first algorithm.\n"; myfile.close();
        std::cout<< "Final result "<< win_rate_first_algorithm << " by first
        algorithm."<<std::endl;
         
    }
    */

    return 0;
}