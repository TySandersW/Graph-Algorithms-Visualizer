#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "header.h"

std::vector<Node> nodes;
std::vector<Edge> edges;

int main() 
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Graph Visualizer");

    while(window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event -> is <sf::Event::Closed>())
                window.close();
            
                if(event -> is <sf::Event::MouseButtonPressed>())
                {
                    auto mouse = event -> getIf <sf::Event::MouseButtonPressed>();

                    if (mouse -> button == sf::Mouse::Button::Left)
                    {
                        mouse_click(window, nodes, *event);
                    }
                }
        }

        

        window.clear();

        draw_edges(window, nodes, edges);
        draw_nodes(window, nodes);

        window.display();
    }
}