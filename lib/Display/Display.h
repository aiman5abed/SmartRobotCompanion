#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "Constants.h"

// The one-and-only TFT instance
extern Adafruit_ILI9341 tft;

void setupDisplay();
void updateHUD(bool returningHome, bool autoMode, float batteryLevel);
void drawGrid(int robotX, int robotY,
              const int dirtGrid[GRID_SIZE][GRID_SIZE],
              const bool obstacleGrid[GRID_SIZE][GRID_SIZE],
              int robotDir);

#endif // DISPLAY_H
