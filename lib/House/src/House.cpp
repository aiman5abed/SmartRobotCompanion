#include "House.h"
#include <Arduino.h>
#include <random>
#include <algorithm>

#define MAX_DIRT_LEVEL      7
#define DIRT_ACCUM_PERIOD_MS 5000  // Dirt increases every 5 seconds

House::House()
    : house_map(GRID_SIZE, std::vector<Cell>(GRID_SIZE)), docking_station({0, 0}) {
    generateRandomGrid();
}

void House::generateRandomGrid() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dirtDist(0, MAX_DIRT_LEVEL);

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            house_map[i][j].dirtLevel   = dirtDist(gen);
            house_map[i][j].isObstacle  = false;
            house_map[i][j].lastCleaned = millis();
        }
    }
    // Clear docking station
    house_map[0][0].dirtLevel   = 0;
    house_map[0][0].isObstacle  = false;
    house_map[0][0].lastCleaned = millis();
    docking_station = {0, 0};
}

void House::toggleObstacle(int x, int y) {
    if (x == 0 && y == 0) return; // never block docking station
    Cell &cell = house_map[x][y];
    cell.isObstacle = !cell.isObstacle;
    if (cell.isObstacle) {
        cell.dirtLevel = 0; // obstacles are always clean
    } else {
        cell.lastCleaned = millis(); // reset dirt timer
    }
}

bool House::isObstacle(int x, int y) const {
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) return true;
    return house_map[x][y].isObstacle;
}

int House::getDirtLevel(int x, int y) const {
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) return -1;
    return house_map[x][y].dirtLevel;
}

void House::cleanDirt(int x, int y) {
    if (isObstacle(x, y)) return;
    Cell &cell = house_map[x][y];
    cell.dirtLevel   = 0;
    cell.lastCleaned = millis();
}

bool House::updateDirtTimers(uint32_t now) {
    bool changed = false;
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            Cell &cell = house_map[i][j];
            if (cell.isObstacle || (i == 0 && j == 0)) continue;
            uint32_t elapsed = now - cell.lastCleaned;
            if (elapsed >= DIRT_ACCUM_PERIOD_MS) {
                int increments = elapsed / DIRT_ACCUM_PERIOD_MS;
                int newLevel = std::min(MAX_DIRT_LEVEL, cell.dirtLevel + increments);
                if (newLevel != cell.dirtLevel) {
                    cell.dirtLevel = newLevel;
                    changed = true;
                }
                cell.lastCleaned += increments * DIRT_ACCUM_PERIOD_MS;
            }
        }
    }
    return changed;
}

int House::getTotalDirt() const {
    int sum = 0;
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (!house_map[i][j].isObstacle)
                sum += house_map[i][j].dirtLevel;
        }
    }
    return sum;
}

std::pair<int, int> House::getDockingStation() const {
    return docking_station;
}
