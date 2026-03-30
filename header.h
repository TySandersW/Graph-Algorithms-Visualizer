#pragma once
#ifndef HEADER_H
#define HEADER_H

#include <vector>
#include <queue>
#include <unordered_map>
#include <SFML/Graphics.hpp>

//Label of a cell
enum class cell
{
    Empty,
    Wall,
    Start,
    Finish,
    Visited,
    Path
};

//Vector2f is a function that carries the position of a node, consisting
//of two float values, in this case an x and y value.
struct Node
{
    sf::Vector2f position;
    cell type;
};


//structures composed of animation definitions for respective algorithms
struct BFSAni 
{
    bool run = false;
    std::queue<int> q;
    std::unordered_map<int, int> parent;
    std::vector<bool> visited;

    int startIndex = 0;
    int finishIndex = 0;
    int rows = 0;
    int cols = 0;
};

struct DFSAni
{
    std::stack<int> s;
    std::vector<bool> visited;
    std::unordered_map<int, int> parent;
    bool run = false;

    int startIndex = 0; 
    int finishIndex = 0;
    int rows = 0;
    int cols = 0;
};

struct DijAni
{
    bool run = false;
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<>> pq;
    std::unordered_map<int, int> parent;
    std::vector<float> distance;
    int startIndex;
    int finishIndex;
    int rows;
    int cols;
};

struct AstarAni
{
    bool run = false;
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<>> pq;
    std::unordered_map<int, int> parent;
    std::vector<bool> visited;
    int startIndex;
    int finishIndex;
    int rows;
    int cols;
};


//Nodes definition
extern std::vector<Node> nodes;


//functions.cpp
void resetGraph(std::vector<Node>& nodes,
                    bool &runningDFS, bool &runningDij, bool &runningAstar,
                    BFSAni &bfsGo, std::stack<int>& dfsStack,
                    std::priority_queue<std::pair<float,int>, std::vector<std::pair<float,int>>, std::greater<>>& dijPQ, std::priority_queue<std::pair<float,int>, std::vector<std::pair<float,int>>, std::greater<>>& astarPQ,
                    std::unordered_map<int,int>& dfsParent, std::unordered_map<int,int>& dijParent, std::unordered_map<int,int>& astarParent);


constexpr float nodeRadius = 16.f;
extern float cellWidth;
extern float cellHeight;
void resetAlgorithms(std::vector<Node>& nodes, 
                    bool &runningDFS, bool &runningDij, bool &runningAstar, 
                    BFSAni &bfsGo);
void mouse_click(sf::RenderWindow &window, std::vector<Node> &nodes, 
                 const sf::Event &event, 
                 bool &moveStart, bool &moveFinish,
                 int &startIndex, int &finishIndex);
int findNode(sf::Vector2f mouse, std::vector<Node>& nodes);
void mouseDrag  (const std::optional<sf::Event>& event, 
                std::vector<Node>& nodes, 
                bool &rightDrag, bool &leftDrag,
                bool &moveStart, bool &moveFinish,
                int &startIndex, int &finishIndex,
                int &last, const int gridWidth,
                sf::RenderWindow &window);

#endif