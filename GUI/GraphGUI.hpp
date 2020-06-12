#pragma once

#include "../Include/BoardGame.hpp"
#include "../Include/Extra/Utilities.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/String.hpp>
#include <iostream>
#include <string>
#include <vector>
//#include "Extra.hpp"

#define INVALID_VERTEX -1
using vertex = int;

double distance(const sf::Vector2i& A, const sf::Vector2i& B)
{
    double X = std::pow(A.x - B.x, 2);
    double Y = std::pow(A.y - B.y, 2);

    return std::sqrt(X + Y);
}

struct Graph_inf
{
    std::vector<std::vector<vertex>> edges;
    std::vector<std::vector<int>> node_positions;
    int num_vertices;
};

struct NodeGUI
{
    NodeGUI(const int radius, int x, int y) : circle(radius), position(x, y)
    {
        circle.setOrigin(sf::Vector2f(radius, radius));
        circle.setPosition(sf::Vector2f(x, y));
        circle.setFillColor(sf::Color::Cyan);
    }

    sf::Vector2i position;
    sf::CircleShape circle;
};

class BoardGraphGUI
{
public:
    explicit BoardGraphGUI(BoardGame& Go)
        : BoardGraph(Go), Fake_edge(sf::Lines, 2), Edges(sf::Lines, 0)
    {
        font.loadFromFile("font.ttf");
        text.setFont(font);
        text.setString("dwd");
        text.setFillColor(sf::Color::Black);
        text.setCharacterSize(24);
    }

    // Build a existent graph.
    explicit BoardGraphGUI(BoardGame& Go, std::vector<sf::Vector2i>& nodes_pos)
        : BoardGraph(Go), Fake_edge(sf::Lines, 2), Edges(sf::Lines, 0)
    {
        font.loadFromFile("font.ttf");
        text.setFont(font);
        text.setString("dwd");
        text.setFillColor(sf::Color::Black);
        text.setCharacterSize(24);

        for (vertex v = 0; v < Go.Board.num_vertices(); ++v)
            add_vertex(nodes_pos[v], false);

        for (auto edge : Go.Board.edges())
        {
            add_edge(edge.from, edge.to, true);
        }

        is_building = false;
    }

    Graph_inf get_graph_GUI()
    {
        Graph_inf graph;

        for (auto edge : BoardGraph.Board.edges())
        {
            graph.edges.push_back({edge.from, edge.to});
        }

        for (auto node : nodes)
        {
            graph.node_positions.push_back({node.position.x, node.position.y});
        }

        graph.num_vertices = nodes.size();

        return graph;
    }

    void add_edge(vertex a, vertex b, bool ignore = false)
    {
        if (BoardGraph.Board.add_edge_no_repeat(a, b) || ignore)
        {
            // Create Edge graphic.
            sf::Vector2f A(nodes[a].position.x, nodes[a].position.y);
            sf::Vector2f B(nodes[b].position.x, nodes[b].position.y);
            Edges.append(A);
            Edges.append(B);
        }
    }

    vertex add_vertex(sf::Vector2i point, bool add_to_graph = true)
    {
        nodes.emplace_back(20, point.x, point.y);
        if (add_to_graph)
            return BoardGraph.Board.add_vertex();
        return nodes.size();
    }

    vertex is_collision_vertex(sf::Vector2i MousePosition)
    {
        for (vertex i = 0; i < nodes.size(); ++i)
        {
            if (distance(nodes[i].position, MousePosition) <=
                nodes[i].circle.getRadius() + 3)
                return i;
        }
        return INVALID_VERTEX;
    }

    void setSelectedNode(vertex v) { selected_node = v; }

    void setEdgeStart(vertex v)
    {
        edge_start = v;
        if (v != INVALID_VERTEX)
            Fake_edge[0].position = nodes[v].circle.getPosition();
    }

    void Update_board()
    {
        BoardGraph.update();
        is_building = false;
    }

    void draw(sf::RenderWindow& window, bool is_finished);
    bool make_action(vertex v);

    vertex edge_start = INVALID_VERTEX;
    // Rules set.
    BoardGame BoardGraph;

private:
    // Specific to the board_score and bla.
    bool is_building = true;
    sf::Font font{};
    sf::Text text{};

    // Variables GUI.
    vertex selected_node = INVALID_VERTEX;
    sf::VertexArray Fake_edge;
    sf::VertexArray Edges;
    std::vector<NodeGUI> nodes;
};

bool operator==(const sf::Vector2f A, const sf::Vector2f B)
{
    return A.x == B.x && A.y == B.y;
}

void BoardGraphGUI::draw(sf::RenderWindow& window, bool is_finished)
{
    sf::Vector2i MousePosition = sf::Mouse::getPosition(window);

    // Show the player.
    // Show current player, otherwise, the score.
    if (!is_building)
    {

        if (!is_finished)
        {
            std::string player = (BoardGraph.player_status() == 'B' ? "Black"
                                                                    : "White");
            text.setString("Player: " + player);
        }
        else
        {
            int black_reward = BoardGraph.reward('B');
            int white_reward = BoardGraph.reward('W');

            std::string player = (black_reward > white_reward ? "Black"
                                                              : "White");
            text.setString(player + " won.");
        }

        window.draw(text);
    }

    if (edge_start != INVALID_VERTEX)
    {
        Fake_edge[1].position = sf::Vector2f(MousePosition.x, MousePosition.y);
        window.draw(Fake_edge);
    }

    if (selected_node != INVALID_VERTEX)
    {
        // Update Edges
        for (int i = 0; i < Edges.getVertexCount(); ++i)
        {
            if (Edges[i].position == nodes[selected_node].circle.getPosition())
                Edges[i].position = sf::Vector2f(MousePosition.x,
                                                 MousePosition.y);
        }

        nodes[selected_node].circle.setPosition(
          sf::Vector2f(MousePosition.x, MousePosition.y));
        nodes[selected_node].position = MousePosition;
    }
    // draw edges

    window.draw(Edges);

    // draw the vertices
    for (int i = 0; i < nodes.size(); ++i)
    {
        sf::Color real_color = nodes[i].circle.getFillColor(),
                  color = nodes[i].circle.getFillColor();
        if (distance(nodes[i].position, MousePosition) <=
            nodes[i].circle.getRadius() + 3)
        {
            color.a = 150;
        }

        nodes[i].circle.setFillColor(color);
        window.draw(nodes[i].circle);
        nodes[i].circle.setFillColor(real_color);
    }
}

// Para garantizar que pudo ejecutar la acción.
bool BoardGraphGUI::make_action(vertex v)
{
    char player = BoardGraph.player_status();
    sf::Color color = sf::Color::Black;
    if (player == 'W')
        color = sf::Color::White;
    if (BoardGraph.is_valid_move(v))
    {

        std::vector<int> nodes_killed = BoardGraph.make_action(v);

        if (v == -1)
            return true;

        nodes[v].circle.setFillColor(color);

        // Update nodes wich are out of board.
        for (int vertice : nodes_killed)
            nodes[vertice].circle.setFillColor(sf::Color::Cyan);

        return true;
    }
    return false;
}
