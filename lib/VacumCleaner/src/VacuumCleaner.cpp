#include "VacuumCleaner.h"
#include <algorithm>
#include <cmath>
#include <iostream>

// Movement and rotation costs (tune as needed)
constexpr float MOVE_COST = 2.0f;
constexpr float ROTATE_COST = 1.5f;
constexpr float CLEAN_COST = 1.0f;
constexpr float CHARGE_RATE = 5.0f; // Battery units per charge cycle

VacuumCleaner::VacuumCleaner(int start_x, int start_y, float max_battery)
    : x(start_x), y(start_y), direction(EAST),
      battery(max_battery), max_battery(max_battery),
      x_DockingStation(start_x), y_DockingStation(start_y)
{
}

void VacuumCleaner::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

void VacuumCleaner::setDirection(Direction dir) {
    direction = dir;
}

int VacuumCleaner::getX() const { return x; }
int VacuumCleaner::getY() const { return y; }
Direction VacuumCleaner::getDirection() const { return direction; }

float VacuumCleaner::getBatteryLevel() const {
    return battery / max_battery;
}

void VacuumCleaner::moveForward() {
    if (battery < MOVE_COST) return;
    switch (direction) {
        case NORTH: if (x > 0) x--; break;
        case EAST:  if (y < GRID_SIZE - 1) y++; break;
        case SOUTH: if (x < GRID_SIZE - 1) x++; break;
        case WEST:  if (y > 0) y--; break;
    }
    battery -= MOVE_COST;
    logStep();
}

void VacuumCleaner::rotateLeft() {
    if (battery < ROTATE_COST) return;
    direction = static_cast<Direction>((direction + 3) % 4);
    battery -= ROTATE_COST;
    logStep();
}

void VacuumCleaner::rotateRight() {
    if (battery < ROTATE_COST) return;
    direction = static_cast<Direction>((direction + 1) % 4);
    battery -= ROTATE_COST;
    logStep();
}

void VacuumCleaner::clean() {
    if (battery < CLEAN_COST) return;
    battery -= CLEAN_COST;
    logStep();
}

void VacuumCleaner::charge() {
    battery = std::min(max_battery, battery + CHARGE_RATE);
}

bool VacuumCleaner::isInDockingStation() const {
    return x == x_DockingStation && y == y_DockingStation;
}

bool VacuumCleaner::isBatteryLow() const {
    return battery < (0.2f * max_battery); // 20% threshold
}

bool VacuumCleaner::isOutOfBattery() const {
    return battery <= 0.0f;
}

void VacuumCleaner::goToDockingStation() {
    x = x_DockingStation;
    y = y_DockingStation;
    direction = EAST;
    logStep();
}

void VacuumCleaner::logStep() {
    all_steps.push_back({x, y, direction});
}

const std::vector<VacuumStep>& VacuumCleaner::getAllSteps() const {
    return all_steps;
}

void VacuumCleaner::resetSteps() {
    all_steps.clear();
    logStep();
}
