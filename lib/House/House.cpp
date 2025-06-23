
// House.cpp
#include "House.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

// One dirt level accumulates every 10 seconds by default
const float House::DIRT_ACCUM_RATE = 0.1f;

House::House() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            dirtLevel[i][j] = std::rand() % (MAX_DIRT_LEVEL + 1);
            obstacleMap[i][j] = false;
            dirtTimer[i][j] = 0.0f;
        }
    }
}

int House::getDirtLevel(int x, int y) const {
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) return 0;
    return dirtLevel[x][y];
}

bool House::isObstacle(int x, int y) const {
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) return false;
    return obstacleMap[x][y];
}

void House::setObstacle(int x, int y, bool status) {
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) return;
    obstacleMap[x][y] = status;
}

void House::resetDirt(int x, int y) {
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) return;
    dirtLevel[x][y] = 0;
    dirtTimer[x][y] = 0.0f;
}

void House::update(float deltaTime) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            dirtTimer[i][j] += deltaTime;
            // Accumulate full levels as timer allows
            float interval = 1.0f / DIRT_ACCUM_RATE;
            while (dirtTimer[i][j] >= interval) {
                dirtTimer[i][j] -= interval;
                if (dirtLevel[i][j] < MAX_DIRT_LEVEL) {
                    ++dirtLevel[i][j];
                }
            }
        }
    }
}
