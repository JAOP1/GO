
#include "GUI/BoardGame.hpp"
#include "GUI/Client.hpp"
#include "GUI/GraphGUI.hpp"
#include "GUI/Include/Extra/Graph.hpp"
#include "GUI/Include/Search_Algorithms/MCTS.hpp"
#include "GUI/Include/Search_Algorithms/MCTS_2.hpp"
#include "GUI/Include/Search_Algorithms/MinMaxSearch.hpp"
#include "GUI/Include/Search_Algorithms/mcts.hpp"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <vector>

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

int main()
{

    /*
        //Declarar grafo
        Graph G(16);
        G.add_edges(
            {
                {0,1},
                {1,2},
                {2,3},
                {4,0},
                {4,5},
                {5,1},
                {5,6},
                {6,2},
                {7,3},
                {7,6},
                {8,4},
                {8,9},
                {9,5},
                {9,10},
                {10,6},
                {10,11},
                {11,7},
                {12,8},
                {12,13},
                {13,9},
                {13,14},
                {14,10},
                {14,15},
                {15,11}
            }
        );

        */
    Graph G(0);
    BoardGame Tablero(G); // Create rules set.
    MCTS MonteCarlo(25, 100, 'B');
    // El mcts y MCTS_2 no dan mejores resultados que MMCTS.
    // MCTS_2 MonteCarlo(25,100,'W');
    // mcts montecarlo(25,100,'W');
    // MinMaxSearch MMS(15,'B');

    bool visualize = true;
    bool compare_algorithms = false;
    // bool is_created_graph = true;

    // Si queremos visualizar las acciones y jugar.
    if (visualize)
    {
        BoardGraphGUI Graph1(Tablero); // Create graph graphic.
        GUI APP(Graph1, 700, 700); // Listener and main window loop.
        APP.Run_VS_AI(MonteCarlo, true); // Play against algorithm.
        // APP.Run(); //Play against someone else.
    }

    /*
        if(compare_algorithms)
        {
            auto win_rate_first_algorithm = evaluate_accuracy(MonteCarloMax,
       MonteCarlo , Tablero , 30);

            //Save result in a extern file.
            std::ofstream myfile;
            myfile.open ("results.txt");
            myfile <<"The MCTS and MCTS2 params: \n"<< "Num simulation: "<< 25
       << "\n Num repetitions: "<<100<<std::endl; myfile <<"Total games played:
       30"<<std::endl; myfile << "Final result "<< win_rate_first_algorithm << "
       by first algorithm.\n"; myfile.close(); std::cout<< "Final result "<<
       win_rate_first_algorithm << " by first algorithm."<<std::endl;

        }
    */
    return 0;
}