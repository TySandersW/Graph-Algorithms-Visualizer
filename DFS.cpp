//
//--DFS function was made just to show how it works, it doesn't look at the 
// whole graph but rather only uses the DFS algorithm to find the end cell.
//
// Its also modified in order to head in the direction of the end cell to find
// it faster, but although quite quick its not the most effeicient route.
//
#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>
#include <cmath>
#include "algorithms.h"
#include "header.h"

//Direction definition
static int dirs[8][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };

//Initializes dfs for animation
void startDFS(std::vector<Node>& nodes, int startIndex, int finishIndex, std::stack<int>& s, std::vector<bool>& visited, std::unordered_map<int, int>& parent)
{
    while (!s.empty()) s.pop();
    visited.assign(nodes.size(), false);
    parent.clear();
    s.push(startIndex);
    visited[startIndex] = true;
}

//deals with the next step of the dfs algorithm, returns true when node is found
bool stepDFS(std::vector<Node>& nodes, int finishIndex, std::stack<int>& s, std::vector<bool>& visited, std::unordered_map<int, int>& parent)
{
    if (s.empty()) return true;

    //Gets the current node, uses LIFO for dfs
    int current = s.top(); s.pop();
    if (current != finishIndex && nodes[current].type != cell::Start)
        nodes[current].type = cell::Visited;

    int rows = sqrt(nodes.size());
    int cols = rows;

    //index to rows and cols
    int r = current / cols;
    int c = current % cols;

    //Look at all neighboring directions
    for (auto& dir : dirs)
    {
        int nr = r + dir[0];
        int nc = c + dir[1];

        if (nr < 0 || nc < 0 || nr >= rows || nc >= cols) continue;
        int next = nr * cols + nc;

        //Prevents diagional between two obstacles
        if (abs(dir[0]) == 1 && abs(dir[1]) == 1)
        {
            int adj1 = r * cols + (c + dir[1]);
            int adj2 = (r + dir[0]) * cols + c;

            if (nodes[adj1].type == cell::Wall || nodes[adj2].type == cell::Wall)
                continue;
        }

        if (!visited[next] && nodes[next].type != cell::Wall)
        {
            parent[next] = current;

            if (next == finishIndex)
                return true;

            visited[next] = true;
            s.push(next);
        }
    }
    return false;
}

//Runs complete dfs without animation
//returns a path from start to finish, not guarenteed shortest
std::vector<int> runDFS(std::vector<Node>& nodes, int startIndex, int finishIndex, int rows, int cols) 
{
    std::stack<int> s;
    std::vector<bool> visited(nodes.size(), false);
    std::unordered_map<int, int> parent;

    s.push(startIndex);
    visited[startIndex] = true;

    while (!s.empty())
    {
        int current = s.top();
        s.pop();

        if (current == finishIndex)
            break;

        //Again turning index into rows and cols
        int r = current / cols;
        int c = current % cols;

        //defines directions
        std::vector<std::pair<int, int>> dirs = 
        {
            {1, 1}, {1, 0}, {0, 1}, {-1, 0},
            {0, -1}, {-1, -1}, {1, -1}, {-1, 1}
        };

        //Reversed so stack sorts in normal order
        std::reverse(dirs.begin(), dirs.end());

        //Look at neighbors
        for (auto [dr, dc] : dirs)
        {
            int nr = r + dr;
            int nc = c + dc;

            //skip out of bounds
            if (nr < 0 || nr >= rows || nc < 0 || nc >= cols)
                continue;
            
            int next = nr * cols + nc;

            //Prevents diagional movement between two walls
            if (abs(dr) == 1 && abs(dc) == 1)
            {
                int adj1 = r * cols + (c + dc);
                int adj2 = (r + dr) * cols + c;

                if (nodes[adj1].type == cell::Wall || nodes[adj2].type == cell::Wall)
                    continue;
            }

            //Process neighbor if valid
            if (!visited[next] && nodes[next].type != cell::Wall)
            {
                visited[next] = true;
                parent[next] = current;
                s.push(next);

                if (nodes[next].type == cell::Empty)
                    nodes[next].type = cell::Visited;
            }
        }
    }

    if (!visited[finishIndex])
        return {};
    
    //Reconstruct path
    std::vector<int> path;
    int current = finishIndex;

    while (current != startIndex)
    {
        path.push_back(current);
        current = parent[current];
    }

    //Add start node and reverse to get correct order
    path.push_back(startIndex);
    std::reverse(path.begin(), path.end());

    return path;
}