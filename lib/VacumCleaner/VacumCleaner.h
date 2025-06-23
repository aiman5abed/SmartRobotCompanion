// VacuumCleaner.h
#ifndef VACUUMCLEANER_H
#define VACUUMCLEANER_H

#include <utility>
#include "House.h"

class VacuumCleaner {
public:
    // Initialize vacuum at home cell (0,0) or custom start
    VacuumCleaner(House* h, int startX = 0, int startY = 0, int startYaw = 0);

    // Query position and orientation
    std::pair<int, int> getPosition() const;
    int getYaw() const;

    // Movement commands
    // Returns true if move successful (enough battery, no obstacle, within bounds)
    bool moveForward();
    void rotateLeft();
    void rotateRight();

    // Clean current cell
    void clean();

    // Battery status
    float getBatteryLevel() const;
    void recharge();

private:
    House* house;
    int x;
    int y;
    int yaw;  // 0 = up, 90 = right, 180 = down, 270 = left
    float batteryLevel;

    static constexpr float MAX_BATTERY = 100.0f;
    static constexpr float MOVE_BATTERY_COST = 1.0f;
    static constexpr float ROTATE_BATTERY_COST = 0.5f;
    static constexpr float CLEAN_BATTERY_COST = 0.2f;
};

#endif // VACUUMCLEANER_H