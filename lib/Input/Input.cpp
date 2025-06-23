#include "Input.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_FT6206.h>

JoyEvent joyEvent{ false, 0 };
static bool prevTouchActive = false;
Adafruit_FT6206 touch = Adafruit_FT6206();

void setupInput() {
  pinMode(JOY_SW, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  Wire.begin();
  touch.begin(40);
}

void readJoystickEvent() {
  int xVal = analogRead(JOY_VRX);
  int yVal = analogRead(JOY_VRY);
  joyEvent.active = false;
  const int lo = 1000, hi = 3000;
  if (xVal < lo)      { joyEvent.active = true; joyEvent.dir = 1; }
  else if (xVal > hi) { joyEvent.active = true; joyEvent.dir = 3; }
  else if (yVal < lo) { joyEvent.active = true; joyEvent.dir = 2; }
  else if (yVal > hi) { joyEvent.active = true; joyEvent.dir = 0; }
}

void readTouchEvent(bool returningHome,
                    bool obstacleGrid[GRID_SIZE][GRID_SIZE]) {
  if (returningHome) return;
  bool curr = touch.touched();
  if (curr && !prevTouchActive) {
    TS_Point p = touch.getPoint();
    int screenX = map(p.y, 0, 320, 0, tft.width()-1);
    int screenY = map(p.x, 0, 240, 0, tft.height()-1);
    if (screenY >= HEADER_HEIGHT) {
      int gx = screenX / CELL_SIZE;
      int gy = (screenY - HEADER_HEIGHT) / CELL_SIZE;
      if (gx>=0 && gx<GRID_SIZE && gy>=0 && gy<GRID_SIZE)
        obstacleGrid[gy][gx] = !obstacleGrid[gy][gx];
    }
  }
  prevTouchActive = curr;
}
