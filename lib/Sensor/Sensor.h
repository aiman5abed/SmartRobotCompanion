// Sensor.h
#ifndef SENSOR_H
#define SENSOR_H

#include <vector>

// Forward declaration
class House;

class Sensor {
public:
    // Sensor constructed with reference to the house environment
    explicit Sensor(House* h);

    // Sense dirt level at a specific cell
    int senseDirtLevel(int x, int y) const;

    // Sense obstacle presence at a specific cell
    bool senseObstacle(int x, int y) const;

    // Optional: retrieve entire dirt map
    std::vector<std::vector<int>> senseAllDirt() const;

    // Optional: retrieve entire obstacle map
    std::vector<std::vector<bool>> senseAllObstacles() const;

private:
    House* house;
};

#endif  // SENSOR_H


