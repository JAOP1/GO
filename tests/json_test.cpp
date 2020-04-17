#include "../GUI/Include/Extra/Graph.hpp"
#include "../GUI/Include/Extra/json_manage.hpp"
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

TEST(Json_Test, Creation)
{
    std::string TestFileName = "Test.json";
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
