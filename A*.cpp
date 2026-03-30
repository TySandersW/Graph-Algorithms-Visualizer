#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <limits>
#include "algorithms.h"
#include "header.h"

static int dirs[8][2] = { {-1,0},{1,0},{0,-1},{0,1}, {-1,-1},{-1,1},{1,-1},{1,1} };

//Euclidean distance heuristic
float heuristic(int curr, int finish, int cols)
{
    int r1 = curr / cols, c1 = curr % cols;
    int r2 = finish / cols, c2 = finish % cols;
    return sqrt((r1-r2)*(r1-r2) + (c1-c2)*(c1-c2));
}

//Initializes A* for animation
void startAstar(std::vector<Node>& nodes, int startIndex, int finishIndex,
                std::priority_queue<std::pair<float,int>,
                std::vector<std::pair<float,int>>, std::greater<>>& pq,
                std::vector<float>& gCost,
                std::unordered_map<int,int>& parent)
{
    while(!pq.empty()) pq.pop();

    //Initialize all gCost aka distance from start to infinity
    gCost.assign(nodes.size(), 1e9);
    parent.clear();
    gCost[startIndex] = 0; //Cost to start is 0
    pq.push({heuristic(startIndex, finishIndex, (int)sqrt(nodes.size())), startIndex});
}

//Performs one step of A* for animation, returns true if finish is reached
bool stepAstar(std::vector<Node>& nodes, int finishIndex,
               std::priority_queue<std::pair<float,int>,
               std::vector<std::pair<float,int>>, std::greater<>>& pq,
               std::vector<float>& gCost,
               std::unordered_map<int,int>& parent)
{
    if(pq.empty()) return true;

    //Get node with the lowest f = g + h
    auto [f, current] = pq.top(); pq.pop();

    //skip oudated entries
    if (f > gCost[current] + heuristic(current, finishIndex, sqrt(nodes.size())))
        return false;

    if(current != finishIndex && nodes[current].type != cell::Start)
        nodes[current].type = cell::Visited;

    //Convert index into row and cols
    int rows = sqrt(nodes.size());
    int cols = rows;
    int r = current / cols;
    int c = current % cols;

    //Look at neighbors
    for(auto& dir : dirs)
    {
        int nr = r + dir[0];
        int nc = c + dir[1];

        //skip out of bounds
        if(nr < 0 || nc < 0 || nr >= rows || nc >= cols) continue;
        int next = nr*cols + nc;

        //Prevent diagional movement between two walls
        if (abs(dir[0]) == 1 && abs(dir[1]) == 1)
        {
            int adj1 = r * cols + (c + dir[1]); 
            int adj2 = (r + dir[0]) * cols + c;

            if (nodes[adj1].type == cell::Wall || nodes[adj2].type == cell::Wall)
                continue;
        }

        //skip walls
        if(nodes[next].type == cell::Wall) continue;

        //movement cost, straight = 1, diagional = square root of 2
        float cost = (dir[0]==0 || dir[1]==0) ? 1.f : 1.414f;

        if (gCost[current] + cost < gCost[next])
        {
            gCost[next] = gCost[current] + cost;
            parent[next] = current;

            if (next == finishIndex)
                return true;

            //Push updated node with f = g + h
            pq.push({gCost[next] + heuristic(next, finishIndex, cols), next});
        }
    }

    return false;
}

//Custom node for pqueue in full A*
struct ANode
{
    int index;
    float f;

    //Min-Heap behavior, where lower f has the higher priority
    bool operator>(const ANode& other) const
    {
        return f > other.f;
    }
};

//Octile distance heuristic, more accurate than Eulidean
float heuristic(int r1, int c1, int r2, int c2)
{
    float dx = std::abs(r2 - r1);
    float dy = std::abs(c2 - c1);

    //combines Manhattan + diagional
    return (dx + dy) + (std::sqrt(2.f) - 2.f) * std::min(dx, dy);
}

//Runs full A* algorith not for animation, returns shortest path
std::vector<int> runAstar(std::vector<Node>& nodes, int startIndex, int finishIndex, int rows, int cols)
{
    //Min-heap priority queue based on f = g + h
    std::priority_queue<ANode, std::vector<ANode>, std::greater<ANode>> pq;

    //gCost = distance from start to node
    std::vector<float> gCost(nodes.size(), std::numeric_limits<float>::infinity());
    std::unordered_map<int, int> parent;

    //predetermine goal coordinates
    int goalR = finishIndex / cols;
    int goalC = finishIndex % cols;

    //initialize start
    gCost[startIndex] = 0.f;
    pq.push({startIndex, 0.f});

    //Movement directions
    std::vector<std::pair<int, int>> dirs = 
    {
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

            //Prevents diagioinal movement between two walls
            if (abs(dr) == 1 && abs(dc) == 1)
            {
                int adj1 = r * cols + (c + dc);
                int adj2 = (r + dr) * cols + c;

                if (nodes[adj1].type == cell::Wall || nodes[adj2].type == cell::Wall)
                    continue;
            }

            //Movement cost
            float cost = (abs(dr) == 1 && abs(dc) == 1) ? std::sqrt(2.f) : 1.f;
            float newG = gCost[current] + cost;

            if (newG < gCost[next])
            {
                gCost[next] = newG;
                parent[next] = current;

                //compute heuristic and total cost
                int nr2 = next / cols;
                int nc2 = next % cols;

                float h = heuristic(nr2, nc2, goalR, goalC);
                float f = newG + h;

                pq.push({next, f});

                if (nodes[next].type == cell::Empty)
                    nodes[next].type = cell::Visited;
            }
        }
    }

    if (gCost[finishIndex] == std::numeric_limits<float>::infinity())
        return {};

    //Reconstruct path
    std::vector<int> path;
    int current = finishIndex;

    while (current != startIndex)
    {
        path.push_back(current);
        current = parent[current];
    }

    path.push_back(startIndex);
    std::reverse(path.begin(), path.end());

    return path;
}