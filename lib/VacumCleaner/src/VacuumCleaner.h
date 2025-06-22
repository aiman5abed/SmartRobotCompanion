// ----------------------------------------------------------------
// VacuumCleaner.h - CORRECTED
// ----------------------------------------------------------------
#ifndef VACUUMCLEANER_H
#define VACUUMCLEANER_H

#include <vector>
#include <cstdint>

#define GRID_SIZE 20

// Orientation
enum Direction { NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3 };

// For logging and path tracking
struct VacuumStep {
    int x, y;
    Direction dir;
};

class VacuumCleaner {
public:
    VacuumCleaner(int start_x, int start_y, float max_battery);
    void setPosition(int newX, int newY);
    void setDirection(Direction dir);
    int getX() const;
    int getY() const;
    Direction getDirection() const;
    float getBatteryLevel() const;
    void moveForward();
    void rotateLeft();
    void rotateRight();
    void clean();
    void charge();
    bool isInDockingStation() const;
    bool isBatteryLow() const;
    // --- FIX: Added missing function declaration ---
    bool isOutOfBattery() const;
    void goToDockingStation();
    const std::vector<VacuumStep>& getAllSteps() const;
    void resetSteps();

private:
    int x, y;
    Direction direction;
    float battery, max_battery;
    int x_DockingStation, y_DockingStation;
    std::vector<VacuumStep> all_steps;
    void logStep();
};

#endif // VACUUMCLEANER_H

