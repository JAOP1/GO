#include "../Include/Extra/Graph.hpp"

namespace graphs
{
// Specific graph structures.
using Vertex = Graph::Vertex;

// The size is n-rows + 1 and m-columns + 1.
Graph Grid(int n, int m)
{
    Graph G((n + 1)*(m + 1));

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < m; ++j)
        {
            Vertex v = i*(m + 1) + j;
            G.add_edge(v, v + 1);
            G.add_edge(v, v + m + 1);
        }
    }

    for (int j = 0; j < m; ++j)
    {
        Vertex v = n*(m + 1) + j;
        G.add_edge(v, v + 1);
    }

    for (int i = 0; i < n; ++i)
    {
        Vertex v = i*(m + 1) + m;
        G.add_edge(v, v + m + 1);
    }

    return G;
}

Graph Complete(int n)
{
    Graph G(n);

    for (Vertex i = 0; i + 1 < n; ++i)
    {
        for (Vertex j = i + 1; j < n; ++j)
        {
            G.add_edge(i, j);
        }
    }

    return G;
}

} // namespace graphs