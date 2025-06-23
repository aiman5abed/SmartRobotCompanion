#include "VacumCleaner.h"

VacumCleaner::VacumCleaner(House* h, int startX, int startY, int startYaw)
    : house(h), x(startX), y(startY), yaw(startYaw), batteryLevel(MAX_BATTERY) {}

std::pair<int, int> VacumCleaner::getPosition() const {
    return {x, y};
}

int VaumCleaner::getYaw() const {
    return yaw;
}

bool VacumCleaner::moveForward() {
    if (batteryLevel < MOVE_BATTERY_COST) return false;
    int dx = 0, dy = 0;
    switch (yaw) {
        case 0:    dy = -1; break;
        case 90:   dx = 1;  break;
        case 180:  dy = 1;  break;
        case 270:  dx = -1; break;
    }
    int nx = x + dx;
    int ny = y + dy;
    if (nx < 0 || nx >= GRID_SIZE || ny < 0 || ny >= GRID_SIZE) return false;
    if (house->isObstacle(nx, ny)) return false;
    x = nx; y = ny;
    batteryLevel -= MOVE_BATTERY_COST;
    return true;
}

void VacumCleaner::rotateLeft() {
    if (batteryLevel < ROTATE_BATTERY_COST) return;
    yaw = (yaw + 270) % 360;
    batteryLevel -= ROTATE_BATTERY_COST;
}

void VacumCleaner::rotateRight() {
    if (batteryLevel < ROTATE_BATTERY_COST) return;
    yaw = (yaw + 90) % 360;
    batteryLevel -= ROTATE_BATTERY_COST;
}

void VacumCleaner::clean() {
    if (batteryLevel < CLEAN_BATTERY_COST) return;
    house->resetDirt(x, y);
    batteryLevel -= CLEAN_BATTERY_COST;
}

float VacumCleaner::getBatteryLevel() const {
    return batteryLevel;
}

void VacumCleaner::recharge() {
    batteryLevel = MAX_BATTERY;
}
