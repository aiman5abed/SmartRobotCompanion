#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "House.h"
#include "VacuumCleaner.h"
#include <list>

// Commands for robot movement
enum Command { FORWARD, LEFT, RIGHT };

// Single movement instruction
struct MovementCommand {
    Command command;
};

// Represents a target cell with its priority score
struct CellPriority {
    int x;
    int y;
    int dirtLevel;
    float priorityScore;
};

// Algorithm objectives
enum AlgorithmObjective { CLEANING, RETURN_HOME };

class Algorithm {
public:
    // Constructor: takes pointers to the House and VacuumCleaner
    Algorithm(House* house, VacuumCleaner* vacuum);

    // Plan the next move/path based on current objective
    void calculateNextMove();

    // Retrieve the current planned path
    std::list<MovementCommand> getCurrentPath() const;

    // Change the algorithm's objective (cleaning vs return)
    void setObjective(AlgorithmObjective objective);

    // Update internal obstacle map when a cell toggles
    void updateObstacleMap(int x, int y, bool hasObstacle);

private:
    AlgorithmObjective currentObjective;
    std::list<MovementCommand> currentPath;

    House* house;
    VacuumCleaner* vacuum;

    bool obstacleMap[GRID_SIZE][GRID_SIZE];

    // Selects the dirty cell with the highest priority score
    CellPriority findHighestPriorityCell() const;

    // Computes path back to docking station
    void calculateReturnPath();

    // A* pathfinding to a target cell
    std::list<MovementCommand> aStarPathfinding(int targetX, int targetY);

    // Heuristic and helper functions for pathfinding
    int manhattanDistance(int x1, int y1, int x2, int y2) const;
    float heuristic(int x1, int y1, int x2, int y2, Direction dir) const;
};

#endif // ALGORITHM_H
