
#include "GUI/BoardGame.hpp"
#include "GUI/Client.hpp"
#include "GUI/GraphGUI.hpp"
#include "GUI/Include/Extra/Graph.hpp"
#include "GUI/Include/Search_Algorithms/MCTS.hpp"
#include "GUI/Include/Search_Algorithms/MinMaxSearch.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

/*
template<class search_algorithm1,class search_algorithm2>
double evaluate_accuracy( const search_algorithm1 player1, const search_algorithm2 player2 , )
{

}
*/

int main()
{

    Graph G(0);
    BoardGame Tablero(G); // Create rules set.
    BoardGraphGUI Graph1(Tablero); // Create graph graphic.
    GUI APP(Graph1, 700, 700); // Listener and main window loop.
    // MinMaxSearch MMS(15,'B');
    MCTS MonteCarlo(50, 250, 'B');
    APP.Run_VS_AI(MonteCarlo, true);
    // APP.Run_VS_AI(MMS,true); //False when the AI is the second player,
    // APP.Run();
    // otherwise, it is first. APP.Run(); //If you want play against someone
    // else.

    return 0;
}