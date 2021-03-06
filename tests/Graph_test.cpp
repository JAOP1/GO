#include "../Include/Extra/Graph.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(Graph_Test, Creation)
{
    Graph G(5);
    ASSERT_EQ(G.num_vertices(), 5);
    auto new_vertex = G.add_vertex();
    ASSERT_EQ(new_vertex, 5);
    for (auto v : G.vertices())
        ASSERT_EQ(G.degree(v), 0);
}

TEST(Graph_Test, Degree)
{
    Graph G(5);
    for (auto v : G.vertices())
        ASSERT_EQ(G.degree(v), 0);

    G.add_edge(0, 1);
    G.add_edge(0, 2);
    G.add_edge(0, 3);

    ASSERT_EQ(G.degree(0), 3);
    ASSERT_EQ(G.degree(1), 1);
    ASSERT_EQ(G.degree(2), 1);
    ASSERT_EQ(G.degree(3), 1);
}

TEST(Graph_Test, AddEdge)
{
    Graph G(3);
    G.add_edge_no_repeat(0, 1);
    ASSERT_EQ(G.num_edges(), 1);

    G.add_edge_no_repeat(0, 1);
    ASSERT_EQ(G.num_edges(), 1);

    G.add_edge(0, 1);
    ASSERT_EQ(G.num_edges(), 2);

    G.add_edge_no_repeat(0, 1);
    ASSERT_EQ(G.num_edges(), 2);
}

TEST(Graph_Test, Vertices)
{
    Graph G(10);
    int i = 0;
    for (auto v : G.vertices())
    {
        ASSERT_EQ(v, i);
        ++i;
    }
}

TEST(Graph_Test, Weights)
{
    Graph G(4);
    G.add_edge(0, 1, 10);
    G.add_edge(1, 2, 11);
    G.add_edge(2, 3, 12);

    ASSERT_EQ(G.edge_value(0, 1), 10);
    ASSERT_EQ(G.edge_value(1, 0), 10);

    ASSERT_EQ(G.edge_value(1, 2), 11);
    ASSERT_EQ(G.edge_value(2, 1), 11);

    ASSERT_EQ(G.edge_value(2, 3), 12);
    ASSERT_EQ(G.edge_value(3, 2), 12);
}

TEST(Graph_Test, CompleteGraph)
{
    int n = 5;
    Graph G = graphs::Complete(n);

    ASSERT_EQ(G.num_vertices(), n);
    ASSERT_EQ(G.num_edges(), n*(n - 1)/2);
    for (auto v : G.vertices())
    {
        ASSERT_EQ(G.degree(v), n - 1);
    }
}
