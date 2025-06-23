#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "Constants.h"
#include "Algorithm.h"

void autoNavigate(Algorithm &algo,
                  int &robotX,int &robotY,int &robotDir,
                  bool &returningHome,float &batteryLevel);

#endif // NAVIGATION_H
