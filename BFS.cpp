#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <unordered_map>
#include "algorithms.h"
#include "header.h"

//Function to declare the bfs structure and begins labeling visited nodes in the queue
void startBFS(BFSAni &state, std::vector<Node>& nodes, int startIndex, int finishIndex, int rows, int cols)
{
    state.run = true;
    state.startIndex = startIndex;
    state.finishIndex = finishIndex;
    state.rows = rows;
    state.cols = cols;
    state.parent.clear();
    state.visited.assign(nodes.size(), false);

    while (!state.q.empty()) state.q.pop();
    state.q.push(startIndex);
    state.visited[startIndex] = true;
}

//defines one step at a time, and returns true if finish node is found
//stops the algorithm if true.
bool stepBFS(BFSAni & state, std::vector<Node>& nodes)
{
    if (!state.run || state.q.empty()) return false;

    //Gets the next node using FIFO
    int current = state.q.front();
    state.q.pop();
    
    if (current == state.finishIndex)
    {
        state.run = false;
        return true;
    }

    //converts index into rows and cols
    int r = current / state.cols;
    int c = current % state.cols;

    //direction definition
    std::vector<std::pair<int, int>> dirs =
    {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}, 
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };


    //Looks at neighboring nodes
    for (auto [dr, dc] : dirs)
    {
        int nr = r + dr, nc = c + dc;
        if (nr < 0 || nr >= state.rows || nc < 0 || nc >= state.cols) continue;

        int next = nr * state.cols + nc;

        //Prevents going diagional between two obstacles
        if (abs(dr) == 1 && abs(dc) == 1)
        {
            int adj1 = r * state.cols + (c + dc);
            int adj2 = (r + dr) * state.cols + c;
            if (nodes[adj1].type == cell::Wall || nodes[adj2].type == cell::Wall)
                continue;
        }

        //Look at unvisited neighbors
        if (!state.visited[next] && nodes[next].type != cell::Wall)
        {
            state.parent[next] = current;

            if (next == state.finishIndex)
            {
                state.run = false;
                return true;
            }

            //Mark as visit and add to queue
            state.visited[next] = true;
            state.q.push(next);

            if (nodes[next].type == cell::Empty)
                nodes[next].type = cell::Visited;
        }
    }

    return false;
}

//Runs bfs not including the animation, returning shortest path
std::vector<int> runBFS(std::vector<Node>& nodes, int startIndex, int finishIndex, int rows, int cols) 
{
    //bfs data structures
    std::queue<int> q;
    std::vector<bool> visited(nodes.size(), false);
    std::unordered_map<int, int> parent;

    q.push(startIndex);
    visited[startIndex] = true;

    while (!q.empty())
    {
        int current = q.front();
        q.pop();
        
        if (current == finishIndex) break;

        //Index to rows and columns
        int r = current / cols;
        int c = current % cols;

        std::vector<std::pair<int, int>> dir = {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1},
            {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
        };

        //Looks at surrounding nodes
        for (auto [dr, dc] : dir)
        {
            //define neighbor position
            int nr = r + dr;
            int nc = c + dc;

            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols)
            {
                int next = nr * cols + nc;

                if (abs(dr) == 1 && abs(dc) == 1)
                {
                    int adj1 = r * cols + (c + dc);
                    int adj2 = (r + dr) * cols + c;

                    if (nodes[adj1].type == cell::Wall || nodes[adj2].type == cell::Wall)
                        continue;
                }

                //Look at valid neighbor
                if (!visited[next] && nodes[next].type != cell::Wall)
                {
                    visited[next] = true;
                    parent[next] = current;
                    q.push(next);

                    if (nodes[next].type == cell::Empty)
                        nodes[next].type = cell::Visited;
                }
            }
        }
    }

    //Path reconstruction
    std::vector<int> path;
    int current = finishIndex;

    //If never found, return empty
    if (parent.find(current) == parent.end())
        return path;

    while (current != startIndex)
    {
        path.push_back(current);
        current = parent[current];
    }

    //Reverse to get correct order
    path.push_back(startIndex);
    std::reverse(path.begin(), path.end());

    return path;
}