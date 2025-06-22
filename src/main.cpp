#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>
#include "House.h"
#include "VacuumCleaner.h"
#include "Algorithm.h"
#include "Sensor.h"
#include "Logger.h"

//------------------------------------------------------------
// Pin & build-time configuration
//------------------------------------------------------------
#define JOY_X_PIN     34
#define JOY_Y_PIN     35
#define JOY_BTN_PIN   32

#define TFT_CS        15
#define TFT_DC        14
#define TFT_RST       13

#define DARKGREY      0x7BEF

// --- Time constants for robot actions as per project spec ---
constexpr uint32_t MOVE_DURATION_MS   = 300;
constexpr uint32_t ROTATE_DURATION_MS = 500;
constexpr uint32_t CLEAN_DURATION_MS  = 1000;

// Colour LUT for dirt shading (levels 0-7)
static const uint16_t DIRTY_COLOUR[8] = {
    ILI9341_BLACK,
    0x03E0,             // Dark Green
    ILI9341_GREEN,
    ILI9341_YELLOW,
    0xFD20,             // Orange
    ILI9341_RED,
    0x8000,             // Maroon
    0xA145              // Brown
};

//------------------------------------------------------------
// Global singletons
//------------------------------------------------------------
Adafruit_ILI9341      tft(TFT_CS, TFT_DC, TFT_RST);
Adafruit_FT6206       ts;
House                 house;
VacuumCleaner         vacuum(0, 0, 2500.0f);
Algorithm             planner(&house, &vacuum);
Sensor                sensor(house, vacuum);
// Global logger instance declared in Logger.cpp

//------------------------------------------------------------
// State Machine Enums
//------------------------------------------------------------
enum class Mode { AUTO, MANUAL, DOCKED, RETURN_HOME, SHUTDOWN };
enum class RobotAction { IDLE, MOVING, ROTATING, CLEANING };

static Mode        currentMode     = Mode::AUTO;
static RobotAction robotState      = RobotAction::IDLE;
static bool        touchEnabled    = true;
static uint32_t    actionStartTime = 0;
static std::list<MovementCommand> currentPath;

//------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------
static void drawGrid();
static void drawCell(int r, int c);
static void drawRobot();
static void handleTouch();
static void updateBatteryBar(float pct);
static void updateDirtAndTimers();

//------------------------------------------------------------
// Arduino SETUP
//------------------------------------------------------------
void setup() {
    // Serial for debug
    Serial.begin(115200);

    // Initialize SPI and SD inside logger
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(ILI9341_BLACK);

    // Initialize logger (TFT + SD)
    logger.begin(tft);

    // Initialize touchscreen
    Wire.begin();
    if (!ts.begin()) {
        logger.log("ERROR: FT6206 not found");
    }

    pinMode(JOY_BTN_PIN, INPUT_PULLUP);

    // Initialize house
    house.generateRandomGrid();

    // Initial render
    drawGrid();
    drawRobot();
    updateBatteryBar(1.0f);

    logger.log("System initialized. Mode: AUTO");
}

//------------------------------------------------------------
// Arduino LOOP
//------------------------------------------------------------
void loop() {
    updateDirtAndTimers();
    if (touchEnabled) handleTouch();

    // Low battery check
    if (vacuum.isBatteryLow() && currentMode != Mode::RETURN_HOME && currentMode != Mode::DOCKED) {
        logger.log("LOW BATTERY! Returning to dock.");
        currentMode   = Mode::RETURN_HOME;
        touchEnabled  = false;
        planner.setObjective(RETURN_HOME);
        currentPath.clear();
        robotState    = RobotAction::IDLE;
    }
    if (vacuum.isOutOfBattery() && currentMode != Mode::DOCKED) {
        logger.log("EMERGENCY: Battery depleted. Halting.");
        currentMode = Mode::SHUTDOWN;
    }

    // Mode toggle via joystick button
    static uint32_t lastBtn = 0;
    if (digitalRead(JOY_BTN_PIN) == LOW && millis() - lastBtn > 500) {
        lastBtn = millis();
        if (currentMode == Mode::AUTO) {
            currentMode = Mode::MANUAL;
            logger.log("Switched to MANUAL mode.");
        } else if (currentMode == Mode::MANUAL) {
            currentMode = Mode::AUTO;
            robotState  = RobotAction::IDLE;
            logger.log("Switched to AUTO mode.");
        }
    }

    // State machine
    switch (currentMode) {
        case Mode::MANUAL: {
            int xADC = analogRead(JOY_X_PIN);
            int yADC = analogRead(JOY_Y_PIN);
            if (yADC < 1000) vacuum.moveForward();
            else if (xADC < 1000) vacuum.rotateLeft();
            else if (xADC > 3000) vacuum.rotateRight();
            break;
        }
        case Mode::AUTO:
        case Mode::RETURN_HOME:
            if (robotState == RobotAction::IDLE) {
                if (currentPath.empty()) {
                    planner.calculateNextMove();
                    currentPath = planner.getCurrentPath();
                    if (currentPath.empty()) {
                        if (currentMode == Mode::RETURN_HOME && vacuum.isInDockingStation()) {
                            currentMode = Mode::DOCKED;
                            logger.log("Docked. Charging.");
                        } else if (currentMode == Mode::AUTO) {
                            logger.log("No path. Returning home.");
                            currentMode = Mode::RETURN_HOME;
                            planner.setObjective(RETURN_HOME);
                        }
                    }
                }
                if (!currentPath.empty()) {
                    MovementCommand cmd = currentPath.front();
                    currentPath.pop_front();
                    actionStartTime = millis();
                    if (cmd.command == FORWARD) {
                        robotState = RobotAction::MOVING;
                        vacuum.moveForward();
                    } else {
                        robotState = RobotAction::ROTATING;
                        if (cmd.command == LEFT) vacuum.rotateLeft(); else vacuum.rotateRight();
                    }
                } else if (house.getDirtLevel(vacuum.getX(), vacuum.getY()) > 0 && currentMode == Mode::AUTO) {
                    robotState = RobotAction::CLEANING;
                    actionStartTime = millis();
                    vacuum.clean();
                    logger.log("Cleaning (%d,%d)", vacuum.getX(), vacuum.getY());
                }
            } else {
                uint32_t elapsed = millis() - actionStartTime;
                if ((robotState == RobotAction::MOVING   && elapsed >= MOVE_DURATION_MS) ||
                    (robotState == RobotAction::ROTATING && elapsed >= ROTATE_DURATION_MS)) {
                    robotState = RobotAction::IDLE;
                } else if (robotState == RobotAction::CLEANING && elapsed >= CLEAN_DURATION_MS) {
                    house.cleanDirt(vacuum.getX(), vacuum.getY());
                    drawCell(vacuum.getX(), vacuum.getY());
                    robotState = RobotAction::IDLE;
                }
            }
            break;
        case Mode::DOCKED:
            vacuum.charge();
            if (vacuum.getBatteryLevel() >= 1.0f) {
                logger.log("Battery full. AUTO mode.");
                currentMode   = Mode::AUTO;
                planner.setObjective(CLEANING);
                touchEnabled  = true;
            }
            break;
        case Mode::SHUTDOWN:
            break;
    }

    drawRobot();
    updateBatteryBar(vacuum.getBatteryLevel());

    static uint32_t lastLogTime = 0;
    if (millis() - lastLogTime > 500) {
        lastLogTime = millis();
        const char* mstr = "";
        switch (currentMode) {
            case Mode::AUTO:        mstr = "AUTO"; break;
            case Mode::MANUAL:      mstr = "MANUAL"; break;
            case Mode::DOCKED:      mstr = "DOCKED"; break;
            case Mode::RETURN_HOME: mstr = "RETURN"; break;
            case Mode::SHUTDOWN:    mstr = "HALTED"; break;
        }
        logger.log("%s | Pos:(%d,%d) | Bat:%.0f%% | Dirt:%d",
                   mstr, vacuum.getX(), vacuum.getY(),
                   vacuum.getBatteryLevel()*100, house.getTotalDirt());
    }

    delay(20);
}

//------------------------------------------------------------
// Helper implementations
//------------------------------------------------------------

void drawGrid() {
    const uint16_t cw = tft.width() / GRID_SIZE;
    const uint16_t ch = tft.height() / GRID_SIZE;
    for (uint8_t r = 0; r < GRID_SIZE; ++r) {
        for (uint8_t c = 0; c < GRID_SIZE; ++c) {
            drawCell(r, c);
            tft.drawRect(c * cw, r * ch, cw, ch, DARKGREY);
        }
    }
}

void drawCell(int r, int c) {
    const uint16_t cw = tft.width() / GRID_SIZE;
    const uint16_t ch = tft.height() / GRID_SIZE;
    uint16_t px = c * cw;
    uint16_t py = r * ch;

    uint16_t color = ILI9341_BLACK;
    if (house.isObstacle(r,c)) {
        color = 0x8410; // Slate
    } else {
        uint8_t d = house.getDirtLevel(r,c);
        if (d > 0) color = DIRTY_COLOUR[d];
    }
    tft.fillRect(px+1, py+1, cw-2, ch-2, color);
}

void drawRobot() {
    static int lastX = -1, lastY = -1;
    const uint16_t cw = tft.width() / GRID_SIZE;
    const uint16_t ch = tft.height() / GRID_SIZE;

    if (lastX != -1) drawCell(lastX, lastY);

    uint16_t cx = vacuum.getY() * cw + cw/2;
    uint16_t cy = vacuum.getX() * ch + ch/2;
    tft.fillCircle(cx, cy, min(cw,ch)/3, ILI9341_CYAN);

    switch (vacuum.getDirection()) {
        case NORTH: cy -= 4; break;
        case EAST:  cx += 4; break;
        case SOUTH: cy += 4; break;
        case WEST:  cx -= 4; break;
    }
    tft.fillCircle(cx, cy, 2, ILI9341_RED);
    lastX = vacuum.getX(); lastY = vacuum.getY();
}

void updateBatteryBar(float pct) {
    pct = max(0.0f, min(1.0f, pct));
    tft.fillRect(0, tft.height()-20, tft.width(), 20, ILI9341_BLACK);
    tft.drawRect(10, tft.height()-17, 200, 10, ILI9341_WHITE);
    uint16_t w = (uint16_t)(198 * pct);
    uint16_t col = (pct > 0.2f) ? ILI9341_GREEN : ILI9341_RED;
    tft.fillRect(11, tft.height()-16, w, 8, col);
}

void handleTouch() {
    if (!ts.touched()) return;
    TS_Point p = ts.getPoint();
    int x = p.x;
    int y = p.y;

    uint8_t row = y / (tft.height() / GRID_SIZE);
    uint8_t col = x / (tft.width() / GRID_SIZE);
    if (row < GRID_SIZE && col < GRID_SIZE && !(row == 0 && col == 0)) {
        house.toggleObstacle(row, col);
        planner.updateObstacleMap(row, col, house.isObstacle(row, col));
        drawCell(row, col);
    }
    delay(100);
}

void updateDirtAndTimers() {
    if (house.updateDirtTimers(millis())) {
        drawGrid();
    }
}
