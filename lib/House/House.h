// House.h
#ifndef HOUSE_H
#define HOUSE_H

static const int GRID_SIZE = 20;

class House {
public:
    // Initialize grid with random dirt levels and no obstacles
    House();

    // Get current dirt level (0–MAX_DIRT_LEVEL)
    int getDirtLevel(int x, int y) const;

    // Check if a cell is an obstacle
    bool isObstacle(int x, int y) const;

    // Toggle or set obstacle status for a cell
    void setObstacle(int x, int y, bool status);

    // Reset dirt in a cell (e.g., after cleaning)
    void resetDirt(int x, int y);

    // Update dirt accumulation over elapsed time (seconds)
    void update(float deltaTime);

private:
    int dirtLevel[GRID_SIZE][GRID_SIZE];
    bool obstacleMap[GRID_SIZE][GRID_SIZE];
    float dirtTimer[GRID_SIZE][GRID_SIZE];

    static const int MAX_DIRT_LEVEL = 7;
    // Dirt accumulation rate: levels per second
    static const float DIRT_ACCUM_RATE;
};

#endif  // HOUSE_H
