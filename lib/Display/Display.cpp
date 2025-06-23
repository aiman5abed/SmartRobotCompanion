#include "Display.h"
#include <Arduino.h>

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

void setupDisplay() {
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(5,5);   tft.print("Mode:");
  tft.setCursor(5,25);  tft.print("Batt:");
}

void updateHUD(bool returningHome, bool autoMode, float batteryLevel) {
  tft.fillRect(90,5,100,20,ILI9341_BLACK);
  tft.setCursor(90,5);
  tft.print(autoMode ? "AUTO" : "MANUAL");
  tft.fillRect(90,25,60,20,ILI9341_BLACK);
  tft.setCursor(90,25);
  tft.printf("%.1f%%", batteryLevel);
  if (returningHome) {
    tft.setCursor(160,5);
    tft.setTextColor(ILI9341_YELLOW);
    tft.print("RTB");
    tft.setTextColor(ILI9341_WHITE);
  }
}

static void drawRobot(int px, int py, int cs, int robotDir) {
  int cx = px + cs/2, cy = py + cs/2;
  tft.fillCircle(cx, cy, cs/2, ILI9341_RED);
  int ox=0, oy=0;
  if (robotDir==0) oy=-1;
  else if (robotDir==1) ox=1;
  else if (robotDir==2) oy=1;
  else if (robotDir==3) ox=-1;
  int w=8, h=6;
  int mx = cx+ox*(cs/2-h/2)-w/2,
      my = cy+oy*(cs/2-h/2)-h/2;
  tft.fillRect(mx, my, w, h, ILI9341_BLACK);
  tft.fillRect(mx+2, my+2, 4, 2, ILI9341_WHITE);
}

void drawGrid(int robotX, int robotY,
              const int dirtGrid[GRID_SIZE][GRID_SIZE],
              const bool obstacleGrid[GRID_SIZE][GRID_SIZE],
              int robotDir)
{
  for (int y = 0; y < GRID_SIZE; y++) {
    for (int x = 0; x < GRID_SIZE; x++) {
      int px = x*CELL_SIZE;
      int py = y*CELL_SIZE + HEADER_HEIGHT;
      if (x==robotX && y==robotY) {
        drawRobot(px, py, CELL_SIZE, robotDir);
      } else if (obstacleGrid[y][x]) {
        tft.fillRect(px, py, CELL_SIZE, CELL_SIZE, ILI9341_BLUE);
      } else {
        int d = dirtGrid[y][x];
        uint8_t r = constrain(255 - d*15, 101,255),
                g = constrain(255 - d*23,  67,255),
                b = constrain(255 - d*30,  33,255);
        tft.fillRect(px, py, CELL_SIZE, CELL_SIZE,
                     tft.color565(r,g,b));
      }
    }
  }
}
