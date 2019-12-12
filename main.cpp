
#include "GUI/BoardGame.hpp"
#include "GUI/Client.hpp"
#include "GUI/GraphGUI.hpp"
#include "GUI/Include/Extra/Graph.hpp"
#include "GUI/Include/MCTS.hpp"
#include "GUI/Include/MinMaxSearch.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

int main()
{

    Graph G(0);
    BoardGame Tablero(G); // Create rules set.
    BoardGraphGUI Graph1(Tablero); // Create graph graphic.
    GUI APP(Graph1, 700, 700); // Listener and main window loop.
    // MinMaxSearch MMS(15,'B');
    MCTS MonteCarlo(100, 1000, 'B');
    APP.Run_VS_AI(MonteCarlo, true);
    // APP.Run_VS_AI(MMS,true); //False when the AI is the second player,
    // otherwise, it is first. APP.Run(); //If you want play against someone else.

    return 0;
}