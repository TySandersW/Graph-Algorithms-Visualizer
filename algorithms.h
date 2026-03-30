#pragma once
#ifndef ALGORITHMS_H
#define ALGORITHM_H

#include <vector>
#include <queue>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "header.h"

//Algorithms
std::vector<int> runBFS(std::vector<Node>& nodes, int startIndex, int finishIndex, int rows, int cols);
std::vector<int> runDFS(std::vector<Node>& nodes, int startIndex, int finishIndex, int rows, int cols);
std::vector<int> runDij(std::vector<Node>& nodes, int startIndex, int finishIndex, int rows, int cols);
std::vector<int> runAstar(std::vector<Node>& nodes, int startIndex, int finishIndex, int rows, int cols);


//Algorithm animations
void startBFS(BFSAni &state, std::vector<Node>& nodes, int startIndex, int finishIndex, int rows, int cols);
bool stepBFS(BFSAni & state, std::vector<Node>& nodes);


void startDFS(std::vector<Node>& nodes, int startIndex, int finishIndex, std::stack<int>& s, std::vector<bool>& visited, std::unordered_map<int, int>& parent);
bool stepDFS(std::vector<Node>& nodes, int finishIndex, std::stack<int>& s, std::vector<bool>& visited, std::unordered_map<int, int>& parent);


void startDij(std::vector<Node>&nodes, int startIndex, std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<>>& pq, std::vector<float>& dist, std::unordered_map<int, int>& parent);
bool stepDij(std::vector<Node>& nodes, int finishIndex, std::priority_queue<std::pair<float,int>, std::vector<std::pair<float,int>>, std::greater<>>& pq, std::vector<float>& dist, std::unordered_map<int,int>& parent);


void startAstar(std::vector<Node>& nodes, int startIndex, int finishIndex, std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<>>& pq, std::vector<float>& gCost, std::unordered_map<int, int>& parent);
bool stepAstar(std::vector<Node>& nodes, int finishIndex, std::priority_queue<std::pair<float,int>, std::vector<std::pair<float,int>>, std::greater<>>& pq, std::vector<float>& gCost, std::unordered_map<int,int>& parent);

#endif