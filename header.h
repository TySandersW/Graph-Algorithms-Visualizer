#pragma once
#ifndef HEADER_H
#define HEADER_H

#include <vector>
#include <SFML/Graphics.hpp>


//general
struct Node
{
    sf::Vector2f position;
};

struct Edge
{
    int from;
    int to;
};

extern std::vector<Node> nodes;
extern std::vector<Edge> edges;


//graph.cpp
void draw_nodes(sf::RenderWindow &window, std::vector<Node>& nodes);
void draw_edges(sf::RenderWindow &window, std::vector<Node>& nodes, std::vector<Edge>& edges);


//functions.cpp
void mouse_click(sf::RenderWindow &window, std::vector<Node> &nodes, const sf::Event &event);
void create_edge(sf::Vector2f mouse, std::vector<Node>& nodes, std::vector<Edge>& edges);
int findNode(sf::Vector2f mouse, std::vector<Node>& nodes);

#endif