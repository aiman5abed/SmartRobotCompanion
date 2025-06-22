#ifndef SENSOR_H
#define SENSOR_H

#include "House.h"
#include "VacuumCleaner.h"

// The Sensor provides the robot with information about its environment
class Sensor {
public:
    // Construct with references to the house and the vacuum cleaner
    Sensor(const House& house, const VacuumCleaner& vacuum);

    // Returns true if there is an obstacle in the specified direction from the robot's current position
    bool detectObstacle(Direction dir) const;

    // Returns the dirt level at the robot's current cell
    int detectDirt() const;

    // Returns the normalized battery level (0.0 - 1.0)
    float detectBattery() const;

    // Returns true if the robot is at the docking station
    bool isAtDockingStation() const;

private:
    const House& house;
    const VacuumCleaner& vacuum;
};

#endif // SENSOR_H
