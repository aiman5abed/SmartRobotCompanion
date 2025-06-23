#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "Constants.h"
#include <Arduino.h>

void rotateLeft(float &batteryLevel, int &robotDir);
void rotateRight(float &batteryLevel, int &robotDir);
void moveForward(int &robotX,int &robotY,
                 int robotDir,float &batteryLevel);
void stepTo(int tx,int ty,
            int &robotX,int &robotY,int &robotDir,
            float &batteryLevel);
void cleanCell(int robotX,int robotY,
               int dirtGrid[GRID_SIZE][GRID_SIZE],
               float &batteryLevel,
               unsigned long lastCleanTime[GRID_SIZE][GRID_SIZE]);

#endif // MOVEMENT_H