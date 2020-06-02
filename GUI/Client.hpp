#pragma once

#include "../Include/BoardGame.hpp"
#include "../Include/Extra/json_manage.hpp"
#include "GraphGUI.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/String.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <iostream>
#include <string>
#include <vector>

class GUI
{
public:
    GUI(BoardGraphGUI& board,
        int width,
        int height,
        const std::string FileName,
        bool is_building_ = true)
        : Go(board)
        , window(sf::VideoMode(width, height), "Go Board")
        , is_building(is_building_)
        , FileName_(FileName)
    {

        if (!m_texture.loadFromFile("fondo.jpg"))
            std::cout << "No se encuentra el archivo llamado fondo" << std::endl;
        else
            Background.setTexture(m_texture);
    }

    void Run()
    {

        bool is_finished = false;
        bool is_playing = true;
        // run the program as long as the window is open
        while (window.isOpen())
        {

            Draw(is_finished);

            if (is_finished)
                is_playing = true;
            else if (!is_building)
                is_finished = Go.BoardGraph.is_complete();

            ClientListener(is_playing);
        }
    }

    template <class search_algorithm>
    void Run_VS_AI(search_algorithm AI_Algorithm, bool player_turn)
    {
        is_first_player = player_turn;
        bool is_finished = false;
        // run the program as long as the window is open
        while (window.isOpen())
        {
            Draw(is_finished);

            if (!is_finished)
            {
                if (is_first_player && !is_building)
                {
                    vertex action = -1;
                    int size_action_set =
                      Go.BoardGraph.get_available_sample_cells(1.0).size();
                    if (size_action_set != 0)
                        action = AI_Algorithm.search(Go.BoardGraph);

                    Go.make_action(action);

                    auto prob = AI_Algorithm.get_probabilities_current_state();
                    for (auto i : prob)
                        std::cout << i << " ";
                    std::cout.put('\n');

                    AI_Algorithm.fit_precompute_tree(action);
                    is_first_player = !is_first_player;
                }
                else
                {
                    // check all the window's events that were triggered since
                    // the last iteration of the loop

                    ClientListener(true);

                    if (v_ != -2 && is_first_player) // Eso significa que no ha
                                                     // hecho acción valida.
                    {
                        std::cout << "Entro aqui" << std::endl;
                        AI_Algorithm.fit_precompute_tree(v_);
                    }
                }
                is_finished = Go.BoardGraph.is_complete();
                continue;
            }

            ClientListener(false);
        }
    }

    void ClientListener(bool is_playing);
    void ClientOnKeyPress(sf::Keyboard::Key key);
    void ClientOnMouseButtonPress(sf::Mouse::Button btn, bool is_playing);
    void ClientOnMouseButtonRelease(sf::Mouse::Button btn);

private:
    BoardGraphGUI Go;
    sf::Sprite Background{};
    sf::Texture m_texture{};
    sf::RenderWindow window;

    std::string FileName_;
    bool is_first_player;
    bool is_building = true;
    bool is_cursor = false;
    vertex v_ = -2;

    void Draw(bool is_finished = false)
    {
        // clear the window with black color
        window.clear(sf::Color::Black);

        window.draw(Background);
        Go.draw(window, is_finished);

        // end the current frame
        window.display();
    }

    void ClientOnEvent(const sf::Event& event, bool is_playing)
    {
        switch (event.type)
        {
        case (sf::Event::Closed):
            std::cout << "Se ha cerrado correctamente." << std::endl;
            window.close();
            break;

        case (sf::Event::MouseEntered):
            is_cursor = true;
            break;

        case (sf::Event::MouseLeft):
            is_cursor = false;
            break;

        case (sf::Event::KeyPressed):
            ClientOnKeyPress(event.key.code);
            break;

        case (sf::Event::MouseButtonPressed):
            ClientOnMouseButtonPress(event.mouseButton.button, is_playing);
            break;

        case (sf::Event::MouseButtonReleased):
            ClientOnMouseButtonRelease(event.mouseButton.button);
            break;
        }
    }

    sf::Vector2i MousePosition()
    {
        sf::Vector2i pos = sf::Mouse::getPosition(window);
        return pos;
    }
};

void GUI::ClientOnKeyPress(sf::Keyboard::Key key)
{
    if (is_building)
    {
        // Finish graph creation.
        if (key == sf::Keyboard::Enter)
        {
            std::cout << "Ha finalizado la construccion del grafo." << std::endl;
            is_building = false;
            Go.Update_board();
        }
        // Save graph in a JSON file.
        else if (key == sf::Keyboard::Num0)
        {
            auto graph_ = Go.get_graph_GUI();
            save_graph_to_json(FileName_, graph_);
        }
    }

    else
    {
        // User pass.
        if (key == sf::Keyboard::Space)
        {
            // std::cout << "Pasa jugador actual." << std::endl;
            Go.make_action(-1);
            is_first_player = !is_first_player;
        }
        // Debug function in graphboard.
        else if (key == sf::Keyboard::Num1)
        {
            Go.BoardGraph.debug_function();
        }
    }
}

void GUI::ClientOnMouseButtonPress(sf::Mouse::Button btn, bool is_playing)
{
    if (!is_playing)
        return;

    if (btn == sf::Mouse::Button::Left)
    {
        vertex v = Go.is_collision_vertex(MousePosition());

        if (is_building)
        {
            // Add nodes
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            {

                std::cout << "Agregar nodo " << Go.add_vertex(MousePosition())
                          << std::endl;
            }
            // Add edges
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            {
                Go.setEdgeStart(v);
            }
            // Move node
            else
            {
                std::cout << "Mover nodo " << v << std::endl;
                Go.setSelectedNode(v);
            }
            return;
        }

        if (v != INVALID_VERTEX)
        {
            // Regresa true si pudo realizar la acción.
            if (Go.make_action(v))
            {
                is_first_player = !is_first_player;
                v_ = v;
                return;
            }
        }
        v_ = -2;
    }
}

void GUI::ClientOnMouseButtonRelease(sf::Mouse::Button btn)
{

    if (btn == sf::Mouse::Button::Left)
    {
        // Add Edge
        vertex v = Go.is_collision_vertex(MousePosition());
        if (v != Go.edge_start && v != INVALID_VERTEX &&
            Go.edge_start != INVALID_VERTEX)
        {
            Go.add_edge(Go.edge_start, v);
            std::cout << "Crear Arista " << v << " " << Go.edge_start
                      << std::endl;
        }

        Go.setEdgeStart(INVALID_VERTEX);
        Go.setSelectedNode(INVALID_VERTEX);
    }
}

void GUI::ClientListener(bool is_playing)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        ClientOnEvent(event, is_playing);
    }
}