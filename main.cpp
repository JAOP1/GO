
#include "GUI/BoardGame.hpp"
#include "GUI/Client.hpp"
#include "GUI/GraphGUI.hpp"
#include "GUI/Include/Extra/Graph.hpp"
#include "GUI/Include/Search_Algorithms/MCTS.hpp"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include "CLI11.hpp"

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

int main(int argc, char **argv)
{
    //Esto es para proobar Ggg.
    CLI::App app{"---- Go generalized ---- \n the target is create a smart search wich gives a great performance in differents graphs. "};

    // Define options
    bool visualize = true; //If you visualize the board then you aren't interested in compare algorithms.
    bool against_player = false;
    bool compare_algorithms = false;
    app.add_option("-v", visualize, "Launch the GUI.");
    app.add_option("-p" , against_player , "Play against the algorithm.");

    CLI11_PARSE(app, argc, argv);

    std::cout<<"Input Parameters: "<<std::endl;
    std::cout <<"Visualize: "<< visualize<<std::endl;
    std::cout<<"Against player: "<< against_player<<std::endl;


    Graph G(0);
    BoardGame Tablero(G); // Create rules set.
    MCTS Black_player(25, 100, 'B');
    MCTS White_player(25,100,'W');




    // Si queremos visualizar las acciones y jugar.
    if (visualize)
    {
        BoardGraphGUI Graph1(Tablero); // Create graph graphic.
        GUI APP(Graph1, 700, 700); // Listener and main window loop.

        if(!against_player)
            APP.Run_VS_AI(Black_player, true); // Play against algorithm.
        else
            APP.Run(); //Play against someone else.
    }

    else
    {
        auto win_rate_first_algorithm = evaluate_accuracy(Black_player,
                                                          White_player, Tablero , 30);

        std::cout<<"The first win in a rate of "<<win_rate_first_algorithm<<std::endl;

        /*
        //Save result in a extern file.
        std::ofstream myfile;
        myfile.open ("results.txt");
        myfile <<"The MCTS and MCTS2 params: \n"<< "Num simulation: "<< 25
               << "\n Num repetitions: "<<100<<std::endl; myfile <<"Total games played:
        30"<<std::endl; myfile << "Final result "<< win_rate_first_algorithm << "
        by first algorithm.\n"; myfile.close(); std::cout<< "Final result "<<
        win_rate_first_algorithm << " by first algorithm."<<std::endl;
         */
    }


    return 0;
}