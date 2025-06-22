#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "House.h"
#include "VacuumCleaner.h"
#include <list>

// Movement and orientation enums
enum Command { FORWARD, LEFT, RIGHT };

// For pathfinding and prioritization
struct MovementCommand { Command command; };

struct CellPriority { int x, y; int dirtLevel; float priorityScore; };

// FSM objectives for the algorithm
enum AlgorithmObjective { CLEANING, RETURN_HOME };

class Algorithm {
public:
    Algorithm(House* house, VacuumCleaner* vacuum);

    void calculateNextMove();
    std::list<MovementCommand> getCurrentPath() const;
    void setObjective(AlgorithmObjective objective);
    void updateObstacleMap(int x, int y, bool hasObstacle);

private:
    AlgorithmObjective currentObjective;
    std::list<MovementCommand> currentPath;
    House* house;
    VacuumCleaner* vacuum;
    bool obstacleMap[GRID_SIZE][GRID_SIZE];

    CellPriority findHighestPriorityCell() const;
    std::list<MovementCommand> aStarPathfinding(int targetX, int targetY);
    void calculateReturnPath();
    int manhattanDistance(int x1, int y1, int x2, int y2) const;
    float heuristic(int x1, int y1, int x2, int y2, Direction dir) const;
};

#endif