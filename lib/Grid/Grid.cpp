#include "Grid.h"

int dirtGrid[GRID_SIZE][GRID_SIZE];
bool obstacleGrid[GRID_SIZE][GRID_SIZE];
unsigned long lastCleanTime[GRID_SIZE][GRID_SIZE];
static unsigned long lastDirtGrow = 0;

void setupGrid() {
  randomSeed(analogRead(0));
  unsigned long now = millis();
  for(int y=0;y<GRID_SIZE;y++){
    for(int x=0;x<GRID_SIZE;x++){
      int init = random(0, MAX_DIRT+1);
      dirtGrid[y][x]      = init;
      lastCleanTime[y][x] = now - init*DIRT_ACCUM_INTERVAL;
      obstacleGrid[y][x]  = false;
    }
  }
}

void updateDirtLevels() {
  unsigned long now = millis();
  if (now - lastDirtGrow < DIRT_ACCUM_INTERVAL) return;
  lastDirtGrow = now;
  for(int y=0;y<GRID_SIZE;y++){
    for(int x=0;x<GRID_SIZE;x++){
      if (!obstacleGrid[y][x] && dirtGrid[y][x] < MAX_DIRT)
        dirtGrid[y][x]++;
    }
  }
}

bool isValid(int x,int y){
  return x>=0 && x<GRID_SIZE &&
         y>=0 && y<GRID_SIZE &&
        !obstacleGrid[y][x];
}
