#include "GUI/Client.hpp"
#include "GUI/GraphGUI.hpp"
#include "Include/BoardGame.hpp"
#include "Include/Extra/External/CLI11.hpp"
#include "Include/Extra/Graph.hpp"
#include "Include/Extra/json_manage.hpp"
#include "Search_Algorithms/MCTS.hpp"
#include "Search_Algorithms/MCT_UCT_RAVE.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/String.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

// Solución parcial....
void run_search(const std::string& player, GUI& client, const BoardGame& G)
{

    if (player == "RAVE")
    {
        MC_RAVE Black_player_RAVE(G, 200, 100, 'B');
        client.Run_VS_AI(Black_player_RAVE, true); // Play against algorithm.
    }
    else if (player == "MCT_UCT")
    {

        MCTS Black_player(G, 25, 100, 'B');
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
    app.add_option(
      "-o",
      output_file,
      "Save Graph visualizer in JSON (default file = Graph_create.json) ");
    app.add_option("-i", input_file, "Load Graph visualizer");
    CLI11_PARSE(app, argc, argv);

    std::cout << "----------------- Go -----------------" << std::endl;
    std::cout << "* Build phase: " << std::endl;
    std::cout << "Add edges:  LShift + click node A + click node B" << std::endl;
    std::cout << "Add nodes:  Ctrl + click on mouse." << std::endl;
    std::cout << "Move node:  Click in mouse on the node." << std::endl;
    std::cout << "Save graph: Keyboard num 0" << std::endl;
    std::cout << "Start game: Keyboard Enter" << std::endl;

    // Si queremos visualizar las acciones y jugar.
    if (visualize)
    {
        if (input_file != "")
        {
            std::tuple<Graph, std::vector<sf::Vector2i>> graph_GUI =
              get_json_to_graph_GUI<sf::Vector2i>(input_file);
            std::vector<sf::Vector2i> arr = std::get<1>(graph_GUI);
            Graph G = std::get<0>(graph_GUI);

            BoardGame ruleset(G);
            BoardGraphGUI Graph_visualizer(ruleset, arr);

            GUI APP(Graph_visualizer, 700, 700, output_file, false);
            run_search(player, APP, ruleset);
        }
        else
        {
            Graph G(0);
            BoardGame ruleset(G); // Create rules set.
            BoardGraphGUI Graph1(ruleset); // Create graph graphic.
            GUI APP(Graph1, 700, 700, output_file); // Listener and main window
                                                    // loop.
            run_search(player, APP, ruleset);
        }
    }

    return 0;
}