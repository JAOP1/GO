#pragma once

#include "BoardGame.hpp"
#include "Include/Extra/Utilities.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
//#include "Extra.hpp"

#define INVALID_VERTEX -1
using vertex = std::int64_t;

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
    BoardGraphGUI(BoardGame& Go)
        : BoardGraph(Go), Fake_edge(sf::Lines, 2), Edges(sf::Lines, 0)
    {
        font.loadFromFile("font.ttf");
        text.setFont(font);
        text.setString("dwd");
        text.setFillColor(sf::Color::Black);
        text.setCharacterSize(24);
    }

    void add_edge(vertex a, vertex b)
    {
        if (BoardGraph.Board.add_edge_no_repeat(a, b))
        {
            // Create Edge graphic.
            sf::Vector2f A(nodes[a].position.x, nodes[a].position.y);
            sf::Vector2f B(nodes[b].position.x, nodes[b].position.y);
            Edges.append(A);
            Edges.append(B);
        }
    }

    vertex add_vertex(sf::Vector2i point)
    {
        nodes.emplace_back(20, point.x, point.y);
        return BoardGraph.Board.add_vertex();
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

    void draw(sf::RenderWindow& window);
    void make_action(vertex v);

    vertex edge_start = INVALID_VERTEX;
    // Rules set.
    BoardGame BoardGraph;

private:
    // Specific to the board_score and bla.
    bool is_building = true;
    sf::Font font;
    sf::Text text;

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

void BoardGraphGUI::draw(sf::RenderWindow& window)
{
    sf::Vector2i MousePosition = sf::Mouse::getPosition(window);

    // Show the player.
    if (!is_building)
    {
        std::string player = (BoardGraph.player_status() == 'B' ? "Black"
                                                                : "White");
        text.setString("Player: " + player);
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

void BoardGraphGUI::make_action(vertex v)
{
    char player = BoardGraph.player_status();
    sf::Color color = sf::Color::Black;
    if (player == 'W')
        color = sf::Color::White;

    if (BoardGraph.is_valid_move(v))
    {

        std::cout << "Accion realizada para el jugador " << player << std::endl;
        nodes[v].circle.setFillColor(color);

        // Update nodes wich are out of board.
        std::vector<int> nodes_killed = BoardGraph.make_action(v);
        for (int vertice : nodes_killed)
            nodes[vertice].circle.setFillColor(sf::Color::Cyan);
    }
}
