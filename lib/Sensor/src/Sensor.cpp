#include "Sensor.h"

// Constructor: references to house and vacuum
Sensor::Sensor(const House& house, const VacuumCleaner& vacuum)
    : house(house), vacuum(vacuum) {}

// Returns true if there is an obstacle in the given direction from the robot's current position
bool Sensor::detectObstacle(Direction dir) const {
    int x = vacuum.getX();
    int y = vacuum.getY();
    switch (dir) {
        case NORTH: return house.isObstacle(x - 1, y);
        case EAST:  return house.isObstacle(x, y + 1);
        case SOUTH: return house.isObstacle(x + 1, y);
        case WEST:  return house.isObstacle(x, y - 1);
        default:    return true;
    }
}

// Returns the dirt level at the robot's current position
int Sensor::detectDirt() const {
    return house.getDirtLevel(vacuum.getX(), vacuum.getY());
}

// Returns the normalized battery level (0.0 - 1.0)
float Sensor::detectBattery() const {
    return vacuum.getBatteryLevel();
}

// Returns true if the robot is currently at the docking station
bool Sensor::isAtDockingStation() const {
    auto dock = house.getDockingStation();
    return (vacuum.getX() == dock.first && vacuum.getY() == dock.second);
}
