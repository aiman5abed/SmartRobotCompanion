// Algorithm.h
#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <list>
#include <utility>
#include <vector>

// Forward declarations
class House;
class VacuumCleaner;

enum class AlgorithmObjective {
    CLEANING,
    RETURN_HOME
};

struct MovementCommand {
    // true = move forward, false = rotate (use angle to indicate direction)
    bool isMove;
    int angle;  // valid if isMove == false: +90 or -90 degrees
};

class Algorithm {
public:
    Algorithm(House* h, VacuumCleaner* v);
    
    // Set the current objective (CLEANING or RETURN_HOME)
    void setObjective(AlgorithmObjective objective);

    // Returns the computed path of movement commands
    const std::list<MovementCommand>& getCurrentPath() const;

    // Calculate next set of commands based on objective
    void calculateNextMove();

private:
    void calculateCleaningPath();
    void calculateReturnPath();

    AlgorithmObjective currentObjective;
    std::list<MovementCommand> currentPath;
    House* house;
    VacuumCleaner* vacuum;
    static const int GRID_SIZE = 20;
    bool obstacleMap[GRID_SIZE][GRID_SIZE];
};

#endif  // ALGORITHM_H

