#pragma once
#include "External/json.hpp"
#include "Graph.hpp"
#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>

using json = nlohmann::json;

namespace json_utils
{
// This give you the graph and positional vector.
template <class T>
std::tuple<Graph, std::vector<T>> get_json_to_graph_GUI(const std::string FileName)
{
    std::cout << "Create graph with the name " << FileName << std::endl;
    std::string Directory = "../Graphs/";
    json j;
    // write prettified JSON to another file
    std::ifstream input_file;
    input_file.open(Directory + FileName, std::ifstream::in);
    input_file >> j;

    int num_vertices = j["num vertices"];
    Graph G(num_vertices);

    std::vector<std::vector<int>> edges = j["edges"];

    for (std::vector<int> edge : edges)
        G.add_edge(edge[0], edge[1]);

    int x, y;
    std::vector<T> nodes_position;
    for (int i = 0; i < num_vertices; ++i)
    {
        x = j["vertex " + std::to_string(i)]["x"];
        y = j["vertex " + std::to_string(i)]["y"];

        nodes_position.emplace_back(x, y);
    }

    input_file.close();
    return std::make_tuple(G, nodes_position);
}

template <class T>
void save_graph_to_json(const std::string FileName, T graph_inf)
{
    std::cout << "Saving graph with the name " << FileName << std::endl;
    std::string Directory = "../Graphs/";
    json j;

    j["num vertices"] = graph_inf.num_vertices;

    for (int i = 0; i < graph_inf.num_vertices; ++i)
    {
        j["vertex " + std::to_string(i)]["x"] = graph_inf.node_positions[i][0];
        j["vertex " + std::to_string(i)]["y"] = graph_inf.node_positions[i][1];
    }

    j["edges"] = graph_inf.edges;

    // write prettified JSON to another file
    std::ofstream output_file;
    output_file.open(Directory + FileName, std::ofstream::trunc);
    output_file << j << std::endl;
    output_file.close();
}

template <class game>
std::vector<game> get_json_to_game_data(const std::string FileName, bool is_training = true)
{
    std::vector<game> games_played;
    json J;
    // write prettified JSON to another file
    std::ifstream input_file;
    input_file.open(FileName, std::ifstream::in);
    input_file >> J;
    std::string mode = "Test";
    std::string Num_games = "Test_Games";
    if(is_training)
    {
        mode = "Train";
        Num_games = "Train_Games";
    }
    int total_recordings = J[Num_games];

    for (int i = 0; i < total_recordings; ++i)
    {
        games_played.emplace_back(
          J[mode]["Game " + std::to_string(i)]["state"],
        //  J["Game " + std::to_string(i)]["probabilities_by_action"],
          J[mode]["Game " + std::to_string(i)]["valid_actions"],
          J[mode]["Game " + std::to_string(i)]["black_reward"]);
    }

    input_file.close();
    return games_played;
}

/*
Game structure has this attributes:
vector actions.
vector of probabilities vectors.
1 is the first player won the game.
*/

template <class episode>
void save_games_recordings_to_json(const std::string FileName,
                                   std::vector<episode> games_played,
                                   double training_percent = 1.0)
{
    json JsonFile;
    int total_games = games_played.size();
    int train_games = total_games * training_percent;
    int game_id = 0;

    JsonFile["Train_Games"] = train_games;
    JsonFile["Test_Games"] = total_games - train_games;
    std::string mode = "Train";

    for (auto G : games_played)
    {
        if(game_id == train_games && mode == "Train")
        {
            mode = "Test";
            game_id = 0;
        }
        
        JsonFile[mode]["Game " + std::to_string(game_id)]["state"] = G.states;
        JsonFile[mode]["Game " + std::to_string(game_id)]["valid_actions"] =
          G.valid_actions;
        //JsonFile["Game " + std::to_string(game_id)]["probabilities_by_action"] =
        //  G.probabilities;
        JsonFile[mode]["Game " + std::to_string(game_id)]["black_reward"] =
          G.black_reward;
        game_id++;
    }

    std::ofstream output_file;
    output_file.open(FileName, std::ofstream::trunc);
    output_file << JsonFile << std::endl;
    output_file.close();
}

} // namespace json_utils