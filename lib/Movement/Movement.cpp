#include "Movement.h"
#include <Arduino.h>
#include <algorithm>

void rotateLeft(float &bat,int &dir){
  dir = (dir+3)%4;
  bat = max(0.0f, bat - BAT_DRAIN_ROTATE);
  Serial.println("rotateLeft");
  delay(ROTATE_DELAY);
}

void rotateRight(float &bat,int &dir){
  dir = (dir+1)%4;
  bat = max(0.0f, bat - BAT_DRAIN_ROTATE);
  Serial.println("rotateRight");
  delay(ROTATE_DELAY);
}

void moveForward(int &x,int &y,int dir,float &bat){
  static const int DX[4]={0,1,0,-1}, DY[4]={-1,0,1,0};
  int nx = x + DX[dir], ny = y + DY[dir];
  if(nx>=0&&nx<GRID_SIZE&&ny>=0&&ny<GRID_SIZE){
    x = nx; y = ny;
    bat = max(0.0f, bat - BAT_DRAIN_MOVE);
    Serial.println("moveForward");
    delay(MOVE_DELAY);
  }
}

void stepTo(int tx,int ty,
            int &x,int &y,int &dir,float &bat){
  int desired;
  if      (tx>x) desired = 1;
  else if (tx<x) desired = 3;
  else if (ty<y) desired = 0;
  else if (ty>y) desired = 2;
  else return;
  int diff = (desired - dir + 4)%4;
  if (diff==1)       rotateRight(bat,dir);
  else if (diff==3)  rotateLeft(bat,dir);
  else if (diff==2){
    rotateRight(bat,dir);
    rotateRight(bat,dir);
  }
  moveForward(x,y,dir,bat);
}

void cleanCell(int x,int y,
               int grid[GRID_SIZE][GRID_SIZE],
               float &bat,
               unsigned long lastClean[GRID_SIZE][GRID_SIZE]){
  int d = grid[y][x];
  if (d <= 0) return;
  float cost = (d <= BAT_DRAIN_CLEAN_THRESH ? 1.0f : 2.0f);
  bat = max(0.0f, bat - cost);
  Serial.printf("Cleaned (%d,%d) lvl=%d\n", x,y, d);
  grid[y][x] = 0;
  lastClean[y][x] = millis();
  delay(CLEAN_DELAY);
}
