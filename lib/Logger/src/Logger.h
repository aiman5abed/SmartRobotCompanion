// Logger.h
#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#ifndef SD_CS
#define SD_CS 5
#endif

// On-screen log configuration
static const uint8_t LOG_X         = 10;
static const uint8_t LOG_Y         = 10;
static const uint8_t LOG_WIDTH     = 300;
static const uint8_t LOG_HEIGHT    = 100;
static const uint8_t LOG_FONT_SIZE = 2;
static const uint8_t MAX_LOG_LINES = LOG_HEIGHT / (8 * LOG_FONT_SIZE);

class Logger {
public:
    // Initialize SPI, TFT display, and SD card
    void begin(Adafruit_ILI9341 &display) {
        // Initialize SPI for TFT and SD
        SPI.begin();
        // Initialize TFT
        tft = &display;
        tft->begin();
        tft->setRotation(1);
        tft->fillScreen(ILI9341_BLACK);
        // Draw log frame
        clearScreen();
        redraw();
        // Initialize SD
        if (!SD.begin(SD_CS)) {
            Serial.println("ERROR: SD init failed!");
            appendLine("ERROR: SD init failed!");
            redraw();
        } else {
            Serial.println("SD initialized.");
            appendLine("SD initialized.");
            redraw();
        }
    }

    template<typename... Args>
    void log(const char* format, Args... args) {
        char buf[128];
        snprintf(buf, sizeof(buf), format, args...);
        // 1) Serial output
        Serial.println(buf);
        // 2) On-screen output
        if (tft) {
            appendLine(buf);
            redraw();
        }
        // 3) SD card output
        File f = SD.open("/log.txt", FILE_APPEND);
        if (f) {
            f.println(buf);
            f.close();
        } else {
            Serial.println("ERROR: cannot open log.txt");
        }
    }

private:
    Adafruit_ILI9341 *tft = nullptr;
    String lines[MAX_LOG_LINES];

    void clearScreen() {
        tft->fillRect(LOG_X - 2, LOG_Y - 2, LOG_WIDTH + 4, LOG_HEIGHT + 4, ILI9341_BLACK);
        tft->drawRect(LOG_X - 2, LOG_Y - 2, LOG_WIDTH + 4, LOG_HEIGHT + 4, ILI9341_WHITE);
        for (uint8_t i = 0; i < MAX_LOG_LINES; i++) lines[i].clear();
    }

    void appendLine(const char *text) {
        memmove(lines, lines + 1, sizeof(String) * (MAX_LOG_LINES - 1));
        lines[MAX_LOG_LINES - 1] = String(text);
    }

    void redraw() {
        tft->setTextSize(LOG_FONT_SIZE);
        tft->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
        for (uint8_t i = 0; i < MAX_LOG_LINES; i++) {
            tft->setCursor(LOG_X, LOG_Y + i * (8 * LOG_FONT_SIZE));
            tft->print(lines[i]);
        }
    }
};

extern Logger logger;

#endif // LOGGER_H

