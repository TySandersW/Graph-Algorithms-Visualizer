#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <queue>
#include <cmath>
#include "algorithms.h"
#include "header.h"

static int dirs[8][2] = { {-1,0},{1,0},{0,-1},{0,1}, {-1,-1},{-1,1},{1,-1},{1,1} };

//Initializes dijkstra's for animation
//Also resets queue, array, and parent following
void startDij(std::vector<Node>& nodes, int startIndex,
                   std::priority_queue<std::pair<float,int>,
                   std::vector<std::pair<float,int>>, std::greater<>>& pq,
                   std::vector<float>& dist, std::unordered_map<int,int>& parent)
{
    while(!pq.empty()) pq.pop();
    dist.assign(nodes.size(), 1e9);
    parent.clear();

    dist[startIndex] = 0;
    pq.push({0.f, startIndex});
}

//Performs one step of Dijkstra's, returns true when node is reached
bool stepDij(std::vector<Node>& nodes, int finishIndex,
                  std::priority_queue<std::pair<float,int>,
                  std::vector<std::pair<float,int>>, std::greater<>>& pq,
                  std::vector<float>& dist, std::unordered_map<int,int>& parent)
{
    if(pq.empty()) return true;

    //Get node with smallest distance, skip outdated cells
    auto [d, current] = pq.top(); pq.pop();
    if(d > dist[current]) return false; 

    //Mark nodes as visited
    if(current != finishIndex && nodes[current].type != cell::Start)
        nodes[current].type = cell::Visited;

    //convert index to rows and cols
    int rows = sqrt(nodes.size());
    int cols = rows;
    int r = current / cols;
    int c = current % cols;

    //Look at all directions
    for(auto& dir : dirs)
    {
        int nr = r + dir[0];
        int nc = c + dir[1];

        //skip out of bounds
        if(nr < 0 || nc < 0 || nr >= rows || nc >= cols) continue;
        int next = nr*cols + nc;

        //Movement cost definitions, straight = 1, diagional = square root of 2
        float cost = (dir[0]==0 || dir[1]==0) ? 1.f : 1.414f;

        //Again prevent diagional between two walls
        if (abs(dir[0]) == 1 && abs(dir[1]) == 1)
        {
            int adj1 = r * cols + (c + dir[1]);
            int adj2 = (r + dir[0]) * cols + c;

            if (nodes[adj1].type == cell::Wall || nodes[adj2].type == cell::Wall)
                continue;
        }

        //skip walls all together
        if(nodes[next].type == cell::Wall) continue;

        //Check if shorter path is found
        if (dist[current] + cost < dist[next])
        {
            dist[next] = dist[current] + cost;
            parent[next] = current;

            if (next == finishIndex)
            {
                return true;
            }

            //Add updated node to pqueue
            pq.push({dist[next], next});
        }
    }

    return false;
}


//Priority queue
struct PQNode
{
    int index;
    float distance;

    //Min-heap definition, smallest distance means highest priority
    bool operator>(const PQNode& other) const
    {
        return distance > other.distance;
    }
};

//Runs full Dijkstr's algorithm not for the animation, returns shortest path
std::vector<int> runDij(std::vector<Node>& nodes, int startIndex, int finishIndex, int rows, int cols) 
{
    //Min-heap priority queue
    std::priority_queue<PQNode, std::vector<PQNode>, std::greater<PQNode>> pq;

    //Distance array set default to infinity, and parent map
    std::vector<float> distance(nodes.size(), std::numeric_limits<float>::infinity());
    std::unordered_map<int, int> parent;

    distance[startIndex] = 0.f;
    pq.push({startIndex, 0.f});

    std::vector<std::pair<int, int>> dirs = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    while (!pq.empty())
    {
        int current = pq.top().index;
        pq.pop();

        if (current == finishIndex)
            break;
        
        int r = current / cols;
        int c = current % cols;

        //Look at neighbors
        for (auto [dr, dc] : dirs)
        {
            int nr = r + dr;
            int nc = c + dc;

            //skip out of bounds
            if (nr < 0 || nr >= rows || nc < 0 || nc >= cols)
                continue;
            
            int next = nr * cols + nc;

            //skip walls
            if (nodes[next].type == cell::Wall)
                continue;
            
            //Prevention of diagional between walls
            if (abs(dr) == 1 && abs(dc) == 1)
            {
                int adj1 = r * cols + (c + dc);
                int adj2 = (r + dr) * cols + c;

                if (nodes[adj1].type == cell::Wall || nodes[adj2].type == cell::Wall)
                    continue;
            }

            //movement cost
            float cost = (abs(dr) == 1 && abs(dc) == 1) ? std::sqrt(2.f) : 1.f;

            float newDist = distance[current] + cost;

            if (newDist < distance[next])
            {
                distance[next] = newDist;
                parent[next] = current;

                //Push updated node into pqueue
                pq.push({next, newDist});

                if (nodes[next].type == cell::Empty)
                    nodes[next].type = cell::Visited;
            }
        }
    }

    if (distance[finishIndex] == std::numeric_limits<float>::infinity())
        return {};
    
    std::vector<int> path;
    int current = finishIndex;

    while (current != startIndex)
    {
        path.push_back(current);
        current = parent[current];
    }

    //Add start and reverse order
    path.push_back(startIndex);
    std::reverse(path.begin(), path.end());
    
    return path;
}