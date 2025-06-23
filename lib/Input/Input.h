#ifndef INPUT_H
#define INPUT_H

#include "Constants.h"
#include <cstdint>

struct JoyEvent {
  bool active;
  uint8_t dir; 
};
extern JoyEvent joyEvent;

void setupInput();
void readJoystickEvent();
void readTouchEvent(bool returningHome,
                    bool obstacleGrid[GRID_SIZE][GRID_SIZE]);

#endif // INPUT_H