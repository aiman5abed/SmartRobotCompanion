#include "Navigation.h"
#include "Movement.h"
#include <Arduino.h>

void autoNavigate(Algorithm &algo,
                  int &robotX,int &robotY,int &robotDir,
                  bool &returningHome,float &batteryLevel)
{
  algo.calculateNextMove();  // fills algo.currentPath
  // then you’d dispatch MovementCommands stored there...
  // or just call algo.stepAlongPath() if you add it.
  // When at dock:
  if (returningHome && robotX==0 && robotY==0) {
    Serial.println("Docked. Recharged.");
    batteryLevel = 100.0f;
    returningHome = false;
  }
}
