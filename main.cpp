#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <queue>
#include <unordered_map>
#include "header.h"
#include "algorithms.h"


//Node definition
std::vector<Node> nodes;


//universial cell space
float cellWidth = 0.f;
float cellHeight = 0.f;


//Algorithm animations declarations
bool runBFSAlg = false;
bool runDFSAlg = false;
bool runDijAlg = false;
bool runAstarAlg = false;


//Function value definitions
std::queue<int> front;
std::vector<bool> visited;
std::stack<int> dfsStack;
std::vector<float> dijDist;
std::vector<float> astarG;

std::unordered_map<int,int> bfsParent, dfsParent, dijParent, astarParent;

std::priority_queue<std::pair<float,int>, std::vector<std::pair<float,int>>, std::greater<>> dijPQ, astarPQ;

BFSAni bfsGo;
DFSAni dfsGo;
DijAni dijGo;
AstarAni astarGo;


//Function for 
void reconstructPath(std::vector<Node>& nodes, int start, int finish,
                     std::unordered_map<int,int>& parent)
{
    int curr = finish;
    while(curr != start)
    {
        if(nodes[curr].type != cell::Finish)
            nodes[curr].type = cell::Path;
        if(parent.find(curr) == parent.end()) break;
        curr = parent[curr];
    }
}


int main() 
{
    //window
    const int gridWidth = 800;
    const int menuWidth = 200;
    const int windowWidth = gridWidth + menuWidth;
    const int windowHeight = 600;
    sf::RenderWindow window(sf::VideoMode({windowWidth, windowHeight}), "Graph Visualizer");

    //number of columns and rows
    //-----HERE IS WHERE YOU CAN CHANGE THE GRID SIZE-----
    const int cols = 50;
    const int rows = 50;
    
    float width = gridWidth / (float)cols;
    float height = windowHeight / (float)rows;

    cellWidth = width * 0.8f;
    cellHeight = height * 0.8f;

    //time for animation
    sf::Clock clock;
    float delay = 0.005f;

    for (int r = 0; r < rows; r++) 
    {
        for (int c = 0; c < cols; c++)
        {
            Node node;
            node.position = sf::Vector2f(c*width + width/2, r*height + height/2);
            node.type = cell::Empty;

            if (r == 0 && c == 0)
                node.type = cell::Start;
            if (r == rows-1 && c == cols-1)
                node.type = cell::Finish;

            nodes.push_back(node);
        }
    }

    //Start and Finish
    int startIndex = 0;
    int finishIndex = rows * cols - 1;

    bool moveStart = false;
    bool moveFinish = false;


    std::vector<std::vector<int>> grid(rows, std::vector<int>(cols, 0));

    bool leftDrag = false;
    bool rightDrag = false;
    int last = -1;

    //Text
    sf::Font font;
    if (!font.openFromFile("Roboto-Regular.ttf"))
    {
        std::cout << "Error loading font\n";
    }

    sf::Text title(font, "Graph Algorithms\nVisualizer", 22);
    title.setPosition({gridWidth + 10, 10});
    sf::Text menu(font, "Control buttons:\nS - Move Start\nF - Move Finish\nW - Wall\n\t(Right click to remove)\nR - Reset", 16);
    menu.setPosition(sf::Vector2f({gridWidth + 10, 80}));
    sf::Text algList(font, "Algoithms:\n1 - BFS\n2 - DFS\n3 - Dijkstra's\n4 - A*", 16);
    algList.setPosition(sf::Vector2f({gridWidth + 10, 220}));
    
    //Game loop
    while(window.isOpen())
    {
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (!event) continue;

            if (event->is<sf::Event::Closed>())
                window.close();



            //Implement right and left clicking for the squares
            mouseDrag(event, nodes, rightDrag, leftDrag, moveStart, moveFinish, startIndex, finishIndex, last, gridWidth, window);

            //Implement ability to move start and finish
            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>())
            {
                //---------------------Start---------------------
                if(keyEvent->code == sf::Keyboard::Key::S)
                {
                    moveStart = true;
                    moveFinish = false;
                }
                //---------------------Finish---------------------
                else if (keyEvent->code == sf::Keyboard::Key::F)
                {
                    moveFinish = true;
                    moveStart = false;
                }

                //---------------------Wall---------------------
                else if (keyEvent->code == sf::Keyboard::Key::W)
                {
                    moveFinish = false;
                    moveStart = false;
                }

                //---------------------reset---------------------
                else if (keyEvent->code == sf::Keyboard::Key::R)
                {
                    resetGraph(nodes, runDFSAlg, runDijAlg, runAstarAlg, bfsGo, dfsStack, dijPQ, astarPQ, dfsParent, dijParent, astarParent);
                    for (auto& n : nodes)
                        n.type = cell::Empty;
                    startIndex = 0;
                    finishIndex = rows * cols - 1;
                    nodes[startIndex].type = cell::Start;
                    nodes[finishIndex].type = cell::Finish;
                }

                //---------------------BFS---------------------
                else if (keyEvent->code == sf::Keyboard::Key::Num1)
                {
                    resetAlgorithms(nodes, runDFSAlg, runDijAlg, runAstarAlg, bfsGo);
                    startBFS(bfsGo, nodes, startIndex, finishIndex, rows, cols);
                }

                //---------------------DFS---------------------
                else if (keyEvent->code == sf::Keyboard::Key::Num2)
                {
                    resetAlgorithms(nodes, runDFSAlg, runDijAlg, runAstarAlg, bfsGo);
                    startDFS(nodes, startIndex, finishIndex, dfsStack, visited, dfsParent);
                    runDFSAlg = true;
                }

                //---------------------Dijkstra's---------------------
                else if (keyEvent->code == sf::Keyboard::Key::Num3)
                {
                    resetAlgorithms(nodes, runDFSAlg, runDijAlg, runAstarAlg, bfsGo);       
                    startDij(nodes, startIndex, dijPQ, dijDist, dijParent);
                    runDijAlg = true;
                }

                //---------------------A*---------------------
                else if (keyEvent->code == sf::Keyboard::Key::Num4)
                {
                    resetAlgorithms(nodes, runDFSAlg, runDijAlg, runAstarAlg, bfsGo);
                    startAstar(nodes, startIndex, finishIndex, astarPQ, astarG, astarParent);
                    runAstarAlg = true;
                }
            }

            //---Operates what a mouse click does---
            mouse_click(window, nodes, *event, moveStart, moveFinish, startIndex, finishIndex);

        }
            //---------animation for algorithms---------
        if (clock.getElapsedTime().asSeconds() > delay)
        {
            if (bfsGo.run) 
            {
                bool finished = stepBFS(bfsGo, nodes);
                if (finished)
                {
                    reconstructPath(nodes, startIndex, finishIndex, bfsGo.parent);
                    bfsGo.run = false;
                }
            }

            if(runDFSAlg)
            {
                bool finished = stepDFS(nodes, finishIndex, dfsStack, visited, dfsParent);
                if(finished) 
                {
                    reconstructPath(nodes, startIndex, finishIndex, dfsParent);
                    runDFSAlg = false;
                }
            }

            if(runDijAlg)
            {
                bool finished = stepDij(nodes, finishIndex, dijPQ, dijDist, dijParent);
                if(finished) 
                {
                    reconstructPath(nodes, startIndex, finishIndex, dijParent);
                    runDijAlg = false;
                }
            }

            if(runAstarAlg)
            {
                bool finished = stepAstar(nodes, finishIndex, astarPQ, astarG, astarParent);
                if(finished) 
                {
                    reconstructPath(nodes, startIndex, finishIndex, astarParent);
                    runAstarAlg = false;
                }
            }

            //ensures animation timing
            clock.restart();
        }

        window.clear(sf::Color::Black);

        //background
        float padding = std::min(width, height) * 0.15f;
        float squareWidth = width - padding;
        float squareHeight = height - padding;

        //Node information
        for (const auto& n : nodes)
        {
            sf::RectangleShape shape(sf::Vector2f(squareWidth, squareHeight));
            shape.setOrigin({squareWidth/2.f, squareHeight/2.f});
            shape.setPosition(n.position);

            //Colors of cells
            sf::Color color;
            switch (n.type)
            {
                case cell::Empty:
                    color = sf::Color(20, 20, 20);
                    break;
                case cell::Wall:
                    color = sf::Color(80, 80, 80);
                    break;
                case cell::Start:
                    color = sf::Color::Green;
                    break;
                case cell::Finish:
                    color = sf::Color::Red;
                    break;
                case cell::Visited:
                    color = sf::Color(255, 253, 208);
                    break;
                case cell::Path:
                    color = sf::Color(255, 215, 0);
                    break;
            }
            
            shape.setFillColor(color);
            window.draw(shape);
        }

        //Manu setup/print
        sf::RectangleShape menuBg(sf::Vector2f(menuWidth, windowHeight));
        menuBg.setPosition(sf::Vector2f(gridWidth, 0));
        menuBg.setFillColor(sf::Color(10, 30, 70));
        window.draw(menuBg);
        window.draw(title);
        window.draw(menu);
        window.draw(algList);

        window.display();
    }
    return 0;
}