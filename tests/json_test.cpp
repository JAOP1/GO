#include "../Include/Extra/Graph.hpp"
#include "../Include/Extra/json_manage.hpp"
#include <SFML/Graphics.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

using vertex = std::int64_t;
struct Graph_inf
{
    std::vector<std::vector<vertex>> edges;
    std::vector<std::vector<int>> node_positions;
    int num_vertices;
};

// Estructura para salvar toda una partida jugada.
struct game
{
    game(std::vector<std::vector<char>> states_,
         std::vector<std::vector<double>> prob,
         std::vector<std::vector<int>> valid_actions_,
         int black_reward_)
        : states(states_)
        , probabilities(prob)
        , valid_actions(valid_actions_)
        , black_reward(black_reward_)
    {}

    std::vector<std::vector<char>> states;
    std::vector<std::vector<double>> probabilities;
    std::vector<std::vector<int>> valid_actions;
    int black_reward;
};

TEST(Json_Test, Creation)
{
    std::string TestFileName = "GraphTest.json";
    Graph_inf G;
    G.num_vertices = 5;
    G.edges = {{0, 1}, {0, 3}, {1, 2}, {1, 3}, {2, 3}, {3, 4}};
    G.node_positions = {{34, 56}, {45, 78}, {21, 43}, {87, 54}, {45, 67}};

    save_graph_to_json(TestFileName, G);

    std::tuple<Graph, std::vector<sf::Vector2i>> Data =
      get_json_to_graph_GUI<sf::Vector2i>(TestFileName);
    Graph D = std::get<0>(Data);
    std::vector<sf::Vector2i> array_positions = std::get<1>(Data);

    ASSERT_EQ(D.num_vertices(), 5);
    ASSERT_EQ(D.num_edges(), 6);

    ASSERT_EQ(array_positions.size(), 5);

    for (int i = 0; i < D.num_vertices(); ++i)
    {
        ASSERT_EQ(array_positions[i].x, G.node_positions[i][0]);
        ASSERT_EQ(array_positions[i].y, G.node_positions[i][1]);
    }

    std::vector<Graph::Edge> edges = D.edges();
    for (int i = 0; i < D.num_edges(); ++i)
    {
        ASSERT_EQ(G.edges[i][0], edges[i].from);
        ASSERT_EQ(G.edges[i][1], edges[i].to);
    }
}

TEST(Json_Test, Game)
{
    /*
    Supongamos que tenemos un grafo completo de 4 nodos.
    */

    std::vector<std::vector<double>> prob;
    prob.push_back({0.25, 0.25, 0.25, 0.25});
    prob.push_back({0, 0.333, 0.333, 0.333});
    prob.push_back({0, 0, 0.5, 0.5});

    std::vector<std::vector<char>> states;
    states.push_back({'N', 'N', 'N', 'N'});
    states.push_back({'B', 'N', 'N', 'N'});
    states.push_back({'B', 'W', 'N', 'N'});

    std::vector<std::vector<int>> valid_actions;
    valid_actions.push_back({0, 1, 2, 3});
    valid_actions.push_back({1, 2, 3});
    valid_actions.push_back({2, 3});

    int reward = 1;

    game G(states, prob, valid_actions, reward);

    std::vector<game> episodes;
    episodes.push_back(G);
    episodes.push_back(G);

    std::string Directory = "../Search_Algorithms/C_49/Data/GameDataTest.json";

    save_games_recordings_to_json(Directory, episodes);

    std::vector<game> episodes_json = get_json_to_game_data<game>(Directory);

    ASSERT_EQ(episodes_json.size(), 2);

    game D = episodes_json[0];

    for (int i = 0; i < D.states.size(); ++i)
    {
        auto state = D.states[i];
        for (int j = 0; j < state.size(); ++j)
            ASSERT_EQ(state[j], states[i][j]);
    }

    for (int i = 0; i < D.probabilities.size(); ++i)
    {
        auto proba = D.probabilities[i];

        for (int j = 0; j < proba.size(); ++j)
            ASSERT_EQ(proba[j], prob[i][j]);
    }
}
