// Algorithm.cpp
#include "Algorithm.h"
#include "House.h"
#include "VacuumCleaner.h"
#include <queue>
#include <tuple>
#include <map>
#include <climits>
#include <cmath>

static const float MOVE_COST = 2.0f;
static const float ROTATION_COST = 1.5f;

Algorithm::Algorithm(House* h, VacuumCleaner* v)
    : currentObjective(AlgorithmObjective::CLEANING),
      currentPath(),
      house(h),
      vacuum(v) {
    // Initialize obstacle map from house
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            obstacleMap[i][j] = house->isObstacle(i, j);
        }
    }
}

void Algorithm::setObjective(AlgorithmObjective objective) {
    currentObjective = objective;
    currentPath.clear();
}

const std::list<MovementCommand>& Algorithm::getCurrentPath() const {
    return currentPath;
}

void Algorithm::calculateNextMove() {
    if (currentObjective == AlgorithmObjective::RETURN_HOME) {
        calculateReturnPath();
    } else {
        calculateCleaningPath();
    }
}

// Helper to build a coverage path prioritizing dirt levels
void Algorithm::calculateCleaningPath() {
    // Placeholder: implement coverage with priority weighting by dirt
    // For now, simple BFS to nearest dirty cell
    // TODO: replace with A* variant factoring in dirt weight
    struct Node { int x, y; float cost; };
    using State = std::pair<float, std::tuple<int,int,int>>;
    // Get current position and orientation
    auto [sx, sy] = vacuum->getPosition();
    int syaw = vacuum->getYaw();
    
    // Min-heap: cost, (x,y,yaw)
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    std::map<std::tuple<int,int,int>, float> dist;
    std::map<std::tuple<int,int,int>, std::tuple<int,int,int>> parent;
    std::map<std::tuple<int,int,int>, MovementCommand> moveCommand;

    pq.push({0.0f, {sx, sy, syaw}});
    dist[{sx, sy, syaw}] = 0.0f;

    std::tuple<int,int,int> targetState; bool found = false;
    // Search until we hit a dirty cell
    while (!pq.empty()) {
        auto [c, state] = pq.top(); pq.pop();
        auto [x,y,yaw] = state;
        if (house->getDirtLevel(x,y) > 0) {
            targetState = state;
            found = true;
            break;
        }
        if (c > dist[state]) continue;
        // Generate moves: forward, rotate left, rotate right
        std::vector<std::pair<MovementCommand, std::tuple<int,int,int>>> nexts;
        // Move forward
        int dx = 0, dy = 0;
        switch (yaw) {
            case 0: dy = -1; break;
            case 90: dx = 1; break;
            case 180: dy = 1; break;
            case 270: dx = -1; break;
        }
        int nx = x + dx;
        int ny = y + dy;
        if (nx>=0 && nx<GRID_SIZE && ny>=0 && ny<GRID_SIZE && !obstacleMap[nx][ny]) {
            MovementCommand mc{true,0};
            nexts.push_back({mc,{nx,ny,yaw}});
        }
        // Rotate left
        MovementCommand left{false,-90};
        nexts.push_back({left,{x,y,(yaw+270)%360}});
        // Rotate right
        MovementCommand right{false,90};
        nexts.push_back({right,{x,y,(yaw+90)%360}});

        for (auto& [cmd, ns] : nexts) {
            float stepCost = cmd.isMove ? MOVE_COST : ROTATION_COST;
            float nd = c + stepCost;
            if (!dist.count(ns) || nd < dist[ns]) {
                dist[ns] = nd;
                parent[ns] = state;
                moveCommand[ns] = cmd;
                pq.push({nd, ns});
            }
        }
    }
    if (!found) return;
    // Reconstruct path of commands
    std::vector<MovementCommand> rev;
    for (auto cur = targetState; cur != std::tuple<int,int,int>{sx,sy,syaw}; ) {
        auto cmd = moveCommand[cur];
        rev.push_back(cmd);
        cur = parent[cur];
    }
    // Commands from start->target, so reverse
    currentPath.clear();
    for (auto it = rev.rbegin(); it != rev.rend(); ++it) {
        currentPath.push_back(*it);
    }
}

// Simple A* to home (0,0)
void Algorithm::calculateReturnPath() {
    struct Node { int x, y; int yaw; float g, f; };
    using State = std::tuple<float, int, int, int>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    std::map<std::tuple<int,int,int>, float> gScore;
    std::map<std::tuple<int,int,int>, std::tuple<int,int,int>> parent;
    std::map<std::tuple<int,int,int>, MovementCommand> moveCommand;

    auto [sx, sy] = vacuum->getPosition();
    int syaw = vacuum->getYaw();
    auto heuristic = [&](int x, int y){ return std::hypot(x, y) * MOVE_COST; };

    pq.push({heuristic(sx,sy), sx, sy, syaw});
    gScore[{sx,sy,syaw}] = 0.0f;

    std::tuple<int,int,int> target;
    bool found = false;

    while (!pq.empty()) {
        auto [f, x, y, yaw] = pq.top(); pq.pop();
        if (x==0 && y==0) { target = {x,y,yaw}; found=true; break; }
        float g = gScore[{x,y,yaw}];
        // Moves: forward, rotate left/right as above
        std::vector<std::pair<MovementCommand,std::tuple<int,int,int>>> nexts;
        int dx=0,dy=0;
        switch(yaw){ case 0: dy=-1; break; case 90: dx=1; break; case 180: dy=1; break; case 270: dx=-1; break; }
        int nx=x+dx, ny=y+dy;
        if(nx>=0 && nx<GRID_SIZE && ny>=0 && ny<GRID_SIZE && !obstacleMap[nx][ny]){
            nexts.push_back({{true,0},{nx,ny,yaw}});
        }
        nexts.push_back({{false,-90},{x,y,(yaw+270)%360}});
        nexts.push_back({{false,90},{x,y,(yaw+90)%360}});

        for(auto& [cmd, ns] : nexts){
            float cost = cmd.isMove ? MOVE_COST : ROTATION_COST;
            float ng = g + cost;
            auto key = ns;
            float prev = gScore.count(key) ? gScore[key] : INFINITY;
            float nf = ng + heuristic(std::get<0>(ns), std::get<1>(ns));
            if(ng < prev){
                gScore[key] = ng;
                parent[key] = {x,y,yaw};
                moveCommand[key] = cmd;
                pq.push({nf, std::get<0>(ns), std::get<1>(ns), std::get<2>(ns)});
            }
        }
    }
    if(!found) return;
    std::vector<MovementCommand> rev;
    for(auto cur=target; cur!=std::tuple<int,int,int>{sx,sy,syaw};){
        rev.push_back(moveCommand[cur]);
        cur = parent[cur];
    }
    currentPath.clear();
    for(auto it=rev.rbegin(); it!=rev.rend(); ++it) currentPath.push_back(*it);
}
