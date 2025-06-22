
// ----------------------------------------------------------------
// House.h - CORRECTED
// ----------------------------------------------------------------
#ifndef HOUSE_H
#define HOUSE_H

#include <vector>
#include <utility>
#include <cstdint>

#define GRID_SIZE 20
#define MAX_DIRT_LEVEL 7

struct Cell {
    int dirtLevel = 0;
    bool isObstacle = false;
    uint32_t lastCleaned = 0;
};

class House {
public:
    House();
    void generateRandomGrid();
    void toggleObstacle(int x, int y);
    bool isObstacle(int x, int y) const;
    int getDirtLevel(int x, int y) const;
    void cleanDirt(int x, int y);
    // --- FIX: Changed return type to bool for efficiency ---
    bool updateDirtTimers(uint32_t now);
    int getTotalDirt() const;
    std::pair<int, int> getDockingStation() const;

private:
    std::vector<std::vector<Cell>> house_map;
    std::pair<int, int> docking_station;
};

#endif // HOUSE_H
