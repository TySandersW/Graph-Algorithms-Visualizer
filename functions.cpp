#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <cmath>
#include "header.h"

void mouse_click(sf::RenderWindow &window, std::vector<Node> &nodes, const sf::Event &event) {
    if (const auto* mouse = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouse -> button == sf::Mouse::Button::Left)
        {
            sf::Vector2f pos = window.mapPixelToCoords(mouse -> position);

            nodes.push_back(Node{pos});
        }
    }
}

int findNode(sf::Vector2f mouse, std::vector<Node>& nodes)
{
    for (int i = 0; i < nodes.size(); i++)
    {
        float dx = mouse.x - nodes[i].position.x;
        float dy = mouse.y - nodes[i].position.y;

        if (sqrt(dx*dx + dy*dy) < 20)
            return i;
    }

    return -1;
}

void create_edge(sf::Vector2f mouse, std::vector<Node>& nodes, std::vector<Edge>& edges)
{
    static int first = -1;

    int click = findNode(mouse, nodes);

    if (click == -1)
        return;
    
    if (first == -1)
    {
        first = click;
    }
    else
    {
        edges.push_back(Edge{first, click});
        first = -1;
    }
}