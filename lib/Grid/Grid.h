#ifndef GRID_H
#define GRID_H

#include "Constants.h"
#include <Arduino.h>

extern int dirtGrid[GRID_SIZE][GRID_SIZE];
extern bool obstacleGrid[GRID_SIZE][GRID_SIZE];
extern unsigned long lastCleanTime[GRID_SIZE][GRID_SIZE];

void setupGrid();
void updateDirtLevels();
bool isValid(int x,int y);

#endif // GRID_H