#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "header.h"

void draw_nodes(sf::RenderWindow &window, std::vector<Node>& nodes) {
    for(int i = 0; i < nodes.size(); i++) 
    {
        sf::CircleShape circle(20);
        circle.setFillColor(sf::Color::Blue);
        circle.setOrigin(sf::Vector2f(20.f, 20.f));
        circle.setPosition(nodes[i].position);

        window.draw(circle);
    }
}

void draw_edges(sf::RenderWindow &window, std::vector<Node>& nodes, std::vector<Edge>& edges) 
{
    for (const Edge& e : edges) 
    {        
        sf::Vertex line[2];

        line[0].position = nodes[e.from].position;
        line[1].position = nodes[e.to].position;
        
        window.draw(line, 2, sf::PrimitiveType::Lines);

    }
}