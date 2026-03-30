#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <cmath>
#include "header.h"
#include "algorithms.h"

//Turns off all algorithm animations and pathways become unvisited again
void resetAlgorithms(std::vector<Node>& nodes,
                     bool &runningDFS, bool &runningDij, bool &runningAstar,
                     BFSAni &bfsGo)
{
    runningDFS = false;
    runningDij = false;
    runningAstar = false;
    bfsGo.run = false;

    for (auto &n : nodes)
    {
        if (n.type == cell::Visited || n.type == cell::Path)
            n.type = cell::Empty;
    }
}


//Initiates a function every time the user clicks the mouse button depending on where
void mouse_click(sf::RenderWindow &window, std::vector<Node> &nodes, 
                 const sf::Event &event, 
                 bool &moveStart, bool &moveFinish,
                 int &startIndex, int &finishIndex)
{
    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>())
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(
            sf::Vector2i(mouseEvent->position.x, mouseEvent->position.y)
        );

        int idx = findNode(mousePos, nodes);
        if (idx == -1) return;

        if (moveStart && idx != finishIndex && nodes[idx].type == cell::Empty)
        {
            nodes[startIndex].type = cell::Empty;
            startIndex = idx;
            nodes[startIndex].type = cell::Start;
            moveStart = false;
        }

        else if (moveFinish && idx != startIndex && nodes[idx].type == cell::Empty)
        {
            nodes[finishIndex].type = cell::Empty;
            finishIndex = idx;
            nodes[finishIndex].type = cell::Finish;
            moveFinish = false;
        }

        else if (mouseEvent->button == sf::Mouse::Button::Left && nodes[idx].type == cell::Empty)
        {
            nodes[idx].type = cell::Wall;
        }

        else if (mouseEvent->button == sf::Mouse::Button::Right && nodes[idx].type == cell::Wall)
        {
            nodes[idx].type = cell::Empty;
        }
    }
}

//Function to find the node based on where the mouse position is
int findNode(sf::Vector2f mouse, std::vector<Node>& nodes)
{
    for (int i = 0; i < nodes.size(); i++)
    {
        float halfWidth = cellWidth / 2.f;
        float halfHeight = cellHeight / 2.f;

        if (mouse.x >= nodes[i].position.x - halfWidth &&
            mouse.x <= nodes[i].position.x + halfWidth &&
            mouse.y >= nodes[i].position.y - halfHeight &&
            mouse.y <= nodes[i].position.y + halfHeight)
        {
            return i;
        }
    }

    return -1;
}

//Function that finds whether the mouse is being dragged
void mouseDrag  (const std::optional<sf::Event>& event, 
                std::vector<Node>& nodes, 
                bool &rightDrag, bool &leftDrag,
                bool &moveStart, bool &moveFinish,
                int &startIndex, int &finishIndex,
                int &last, const int gridWidth,
                sf::RenderWindow &window)
{
    if (!event)
        return;
    
    if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseEvent->button == sf::Mouse::Button::Left)
            leftDrag = true;
        if (mouseEvent->button == sf::Mouse::Button::Right)
            rightDrag = true;
    }

    if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonReleased>())
    {
        if (mouseEvent->button == sf::Mouse::Button::Left)
            leftDrag = false;
        if (mouseEvent->button == sf::Mouse::Button::Right)
            rightDrag = false;
    }


    static sf::Vector2f prevMousePos(-1, -1);

    //Moves the start, the finish, or creates or removes walls depending on the context
    //This will deal with the current mouse position
    if (leftDrag || rightDrag)
    {
        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

        float dx = mousePos.x - prevMousePos.x;
            float dy = mousePos.y - prevMousePos.y;
            float dist = std::sqrt(dx*dx + dy*dy);

            float step = std::min(cellWidth, cellHeight) / 2.f;
            int steps = std::max(1, (int)(dist / step));

        if (mousePos.x < gridWidth)
            {
                int idx = findNode(mousePos, nodes);
                if (idx != -1)
                {
                    if (moveStart)
                    {
                        if (idx != startIndex && idx != finishIndex && nodes[idx].type == cell::Empty)
                        {
                            nodes[startIndex].type = cell::Empty;
                            startIndex = idx;
                            nodes[startIndex].type = cell::Start;
                        }
                    }
                    else if (moveFinish)
                    {
                        if (idx != finishIndex && idx != startIndex && nodes[idx].type == cell::Empty)
                        {
                            nodes[finishIndex].type = cell::Empty; 
                            finishIndex = idx;
                            nodes[finishIndex].type = cell::Finish; 
                        }
                    }
                    else
                    {
                        if (leftDrag && nodes[idx].type == cell::Empty)
                        {
                            nodes[idx].type = cell::Wall;
                        }
                        else if (rightDrag && nodes[idx].type == cell::Wall)
                        {
                            nodes[idx].type = cell::Empty;
                        }
                    }
                }
            }

            //These definitions handle drag smoothing, filling in all
            //missed cells when dragging, due to the problem of the
            //mouse position moving faster than the number of frames per
            //second. Without ths part, the dragging would be very choppy
        if (prevMousePos.x != -1)
        {
            for (int i = 0; i <= steps; i++)
            {
                float t = i / (float)steps;
                sf::Vector2f interpPos = prevMousePos + t * sf::Vector2f(dx, dy);

                if (interpPos.x < gridWidth)
                {
                    int idx = findNode(interpPos, nodes);
                    if (idx != -1)
                    {
                        if (moveStart && idx != startIndex && idx != finishIndex && nodes[idx].type == cell::Empty)
                        {
                            nodes[startIndex].type = cell::Empty;
                            startIndex = idx;
                            nodes[startIndex].type = cell::Start;
                        }
                        else if (moveFinish && idx != finishIndex && idx != startIndex && nodes[idx].type == cell::Empty)
                        {
                            nodes[finishIndex].type = cell::Empty;
                            finishIndex = idx;
                            nodes[finishIndex].type = cell::Finish;
                        }
                        else
                        {
                            if (leftDrag && nodes[idx].type == cell::Empty)
                                nodes[idx].type = cell::Wall;
                            else if (rightDrag && nodes[idx].type == cell::Wall)
                                nodes[idx].type = cell::Empty;
                        }
                    }
                }
            }
        }

        prevMousePos = mousePos;
    }
    else
    {
        prevMousePos = sf::Vector2f(-1, -1);
    }
}

//This long winded function stops the animation of any algorithm and
//resets the graph back to its original position
void resetGraph(std::vector<Node>& nodes,
                    bool &runningDFS,
                    bool &runningDij,
                    bool &runningAstar,
                    BFSAni &bfsGo,
                    std::stack<int>& dfsStack,
                    std::priority_queue<std::pair<float,int>,
                    std::vector<std::pair<float,int>>, std::greater<>>& dijPQ,
                    std::priority_queue<std::pair<float,int>,
                    std::vector<std::pair<float,int>>, std::greater<>>& astarPQ,
                    std::unordered_map<int,int>& dfsParent,
                    std::unordered_map<int,int>& dijParent,
                    std::unordered_map<int,int>& astarParent)
{
    runningDFS = false;
    runningDij = false;
    runningAstar = false;
    bfsGo.run = false;

    while (!bfsGo.q.empty()) bfsGo.q.pop();
    bfsGo.parent.clear();

    while (!dfsStack.empty()) dfsStack.pop();
    dfsParent.clear();

    while (!dijPQ.empty()) dijPQ.pop();
    dijParent.clear();

    while (!astarPQ.empty()) astarPQ.pop();
    astarParent.clear();

    for (auto &n : nodes)
    {
        if (n.type == cell::Visited || n.type == cell::Path)
            n.type = cell::Empty;
    }
}