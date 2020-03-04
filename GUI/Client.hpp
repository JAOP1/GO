#pragma once

#include "BoardGame.hpp"
#include "GraphGUI.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

class GUI
{
public:
    GUI(BoardGraphGUI& board, int width, int height , bool is_building_ = false)
        :   Go(board),
            window(sf::VideoMode(width, height), "Go Board"),
            is_building(is_building_) //Esto aun no funciona. (pronto...)
    {

        if (!m_texture.loadFromFile("fondo.jpg"))
            std::cout << "No se encuentra el archivo llamado fondo" << std::endl;
        else
            Background.setTexture(m_texture);
    }

    void Run()
    {

        // run the program as long as the window is open
        while (window.isOpen())
        {
            // check all the window's events that were triggered since the last
            // iteration of the loop
            sf::Event event;
            while (window.pollEvent(event))
            {
                ClientOnEvent(event);
            }

            Draw();
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
                    std::cout << "Algoritmo pensó en esta accion " << action
                              << std::endl;
                    is_first_player = !is_first_player;
                    // Go.BoardGraph.show_array();
                }
                else
                {
                    // check all the window's events that were triggered since
                    // the last iteration of the loop

                    ClientListener();
                }
                is_finished = Go.BoardGraph.is_complete();
            }

            ClientListener();
            Draw(is_finished);
        }
    }

    void ClientListener();
    void ClientOnKeyPress(sf::Keyboard::Key key);
    void ClientOnMouseButtonPress(sf::Mouse::Button btn);
    void ClientOnMouseButtonRelease(sf::Mouse::Button btn);

private:
    BoardGraphGUI Go;
    sf::Sprite Background;
    sf::Texture m_texture;
    sf::RenderWindow window;

    bool is_first_player;
    bool is_building = true;
    bool is_cursor = false;

    void Draw(bool is_finished = false)
    {
        // clear the window with black color
        window.clear(sf::Color::Black);

        window.draw(Background);
        Go.draw(window, is_finished);

        // end the current frame
        window.display();
    }

    void ClientOnEvent(const sf::Event& event)
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
            ClientOnMouseButtonPress(event.mouseButton.button);
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
    if (is_building && key == sf::Keyboard::Enter)
    {
        std::cout << "Ha finalizado la construccion del grafo." << std::endl;
        is_building = false;
        Go.Update_board();
    }
    // Caso cuando pasa jugador.
    else if (!is_building)
    {
        if (key == sf::Keyboard::Space)
        {
            std::cout << "Pasa jugador actual." << std::endl;
            Go.make_action(-1);
            is_first_player = !is_first_player;
        }
        else if (key == sf::Keyboard::Num1)
        {
            // Utilizaré esta tecla para debug.
            Go.BoardGraph.debug_function();
        }
    }
}

void GUI::ClientOnMouseButtonPress(sf::Mouse::Button btn)
{
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
                is_first_player = !is_first_player;
        }
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

void GUI::ClientListener()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        ClientOnEvent(event);
    }
}