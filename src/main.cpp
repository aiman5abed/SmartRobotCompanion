#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include "Constants.h"
#include "Logger.h"
#include "Sensor.h"
#include "House.h"
#include "VacuumCleaner.h"
#include "Algorithm.h"

#include "Display.h"
#include "Input.h"
#include "Grid.h"
#include "Movement.h"
#include "Navigation.h"

House         house;
VacuumCleaner vacuum(0,0,100.0f);
Sensor        sensor(house, vacuum);
Algorithm     algo(&house, &vacuum);

bool autoMode      = false;
bool returningHome = false;
int  robotX = 0, robotY = 0, robotDir = NORTH;
float batteryLevel = 100.0f;
static bool lastBtn = HIGH;
static unsigned long lastBgDrain = 0;

void setup() {
  Serial.begin(115200);
  delay(100);
  setupDisplay();
  setupInput();
  setupGrid();
  vacuum.resetSteps();
}

void loop() {
  // 1) Toggle AUTO/MANUAL
  bool curBtn = digitalRead(JOY_SW);
  if (lastBtn==HIGH && curBtn==LOW) {
    autoMode = !autoMode;
    Serial.println(autoMode ? "Switched to AUTO" : "Switched to MANUAL");
  }
  lastBtn = curBtn;

  // 2) Read inputs
  readJoystickEvent();
  readTouchEvent(returningHome, obstacleGrid);

  // 3) Background drain
  if (millis() - lastBgDrain >= BAT_DRAIN_BG_INTERVAL) {
    lastBgDrain = millis();
    batteryLevel = max(0.0f, batteryLevel - BAT_DRAIN_BG_AMOUNT);
    Serial.println("Background drain");
  }

  // 4) Act
  if (joyEvent.active && !autoMode) {
    int dx[4]={0,1,0,-1}, dy[4]={-1,0,1,0};
    stepTo(robotX + dx[joyEvent.dir],
           robotY + dy[joyEvent.dir],
           robotX, robotY, robotDir,
           batteryLevel);
  }
  else if (autoMode) {
    autoNavigate(algo,
                 robotX, robotY, robotDir,
                 returningHome, batteryLevel);
  }

  // 5) Clean, check battery, grow dirt
  cleanCell(robotX,robotY,
            dirtGrid, batteryLevel,
            lastCleanTime);

  if (batteryLevel <= BAT_LOW_THRESHOLD && !returningHome) {
    returningHome = true;
    Serial.println("Battery low – heading home");
  }

  updateDirtLevels();

  // 6) Render
  updateHUD(returningHome, autoMode, batteryLevel);
  drawGrid(robotX,robotY,
           dirtGrid, obstacleGrid,
           robotDir);

  delay(50);
}
