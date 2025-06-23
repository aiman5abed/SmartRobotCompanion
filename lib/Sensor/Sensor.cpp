#include "Sensor.h"
#include "House.h"

Sensor::Sensor(House* h)
    : house(h) {}

int Sensor::senseDirtLevel(int x, int y) const {
    return house->getDirtLevel(x, y);
}

bool Sensor::senseObstacle(int x, int y) const {
    return house->isObstacle(x, y);
}

std::vector<std::vector<int>> Sensor::senseAllDirt() const {
    std::vector<std::vector<int>> map;
    map.resize(GRID_SIZE, std::vector<int>(GRID_SIZE));
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            map[i][j] = house->getDirtLevel(i, j);
        }
    }
    return map;
}

std::vector<std::vector<bool>> Sensor::senseAllObstacles() const {
    std::vector<std::vector<bool>> map;
    map.resize(GRID_SIZE, std::vector<bool>(GRID_SIZE));
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            map[i][j] = house->isObstacle(i, j);
        }
    }
    return map;
}
