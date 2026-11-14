// ==============================================================
// Shenzhen Explorer Cyber-Badge Firmware
// Author: Arfat Ansari
// Built for Hack Club Fallen & Maker Faire Shenzhen 2026
// Hardware: ESP32-S3 + 1.69" ST7789 IPS LCD + 6 Buttons + Buzzer
// ==============================================================

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "include/pins.h"
#include "include/audio.h"
#include "include/input.h"
#include "include/invaders.h"
#include "include/snake.h"
#include "include/breakout.h"
#include "include/flappy.h"
#include "include/racer.h"
#include "include/tetris.h"
#include "include/radar.h"
#include "include/telemetry.h"

// State machine for switching between the main menu and games
enum AppState {
    STATE_BOOT,
    STATE_MENU,
    STATE_INVADERS,
    STATE_SNAKE,
    STATE_BREAKOUT,
    STATE_FLAPPY,
    STATE_RACER,
    STATE_TETRIS,
    STATE_RADAR,
    STATE_DIAGNOSTICS
};

TFT_eSPI tft = TFT_eSPI();
InputController input;

// Game and app instances
ShenzhenInvaders invadersGame;
CyberSnake snakeGame;
ShenzhenBreakout breakoutGame;
FlappyDrone flappyGame;
ShenzhenRacer racerGame;
CyberTetris tetrisGame;
CyberRadar cyberRadar;
TelemetryBroadcaster dockTelemetry;

AppState currentState = STATE_BOOT;
int menuSelection = 0;
const int MENU_ITEMS_COUNT = 8;
const char* menuItems[MENU_ITEMS_COUNT] = {
    "1. CYBER INVADERS",
    "2. CYBER SNAKE",
    "3. CYBER BREAKOUT",
    "4. FLAPPY DRONE",
    "5. HIGHWAY RACER",
    "6. CYBER BLOCKS",
    "7. 2.4GHz RF RADAR",
    "8. BADGE DIAGNOSTICS"
};

void drawBootScreen() {
    tft.fillScreen(TFT_BLACK);
    
    // Cyberpunk border frame
    tft.drawRect(10, 20, 220, 240, TFT_CYAN);
    tft.drawRect(12, 22, 216, 236, TFT_NAVY);

    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawCentreString("CYBER EXPLORER", 120, 45, 4);
    
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString("CYBER-ARCADE v2.0", 120, 80, 2);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawCentreString("ARFAT ANSARI // 2026", 120, 115, 2);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("SHENZHEN MAKER FAIRE", 120, 150, 2);

    tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
    tft.drawCentreString("PRESS ANY BUTTON", 120, 210, 2);

    // Play a friendly boot chime
    SoundEngine::playStartupFanfare();
}

void drawMenu() {
    tft.fillScreen(TFT_BLACK);

    // Header bar
    tft.fillRect(0, 0, 240, 32, TFT_NAVY);
    tft.drawFastHLine(0, 32, 240, TFT_CYAN);

    tft.setTextColor(TFT_CYAN, TFT_NAVY);
    tft.drawString("SHENZHEN EXPLORER", 10, 8, 2);

    tft.setTextColor(TFT_YELLOW, TFT_NAVY);
    tft.drawString("v2.0", 200, 8, 2);

    // Render list of apps and games
    for (int i = 0; i < MENU_ITEMS_COUNT; i++) {
        int y = 44 + (i * 26);
        if (i == menuSelection) {
            // Selected item has cyan highlight box
            tft.fillRect(8, y - 2, 224, 22, TFT_CYAN);
            tft.setTextColor(TFT_BLACK, TFT_CYAN);
            tft.drawString("> " + String(menuItems[i]), 12, y + 2, 2);
        } else {
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.drawString("  " + String(menuItems[i]), 12, y + 2, 2);
        }
    }

    // Bottom help bar
    tft.drawFastHLine(0, 256, 240, TFT_DARKGREY);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("UP/DN: Move   A: Play", 20, 262, 2);
}

void drawDiagnostics() {
    tft.fillScreen(TFT_BLACK);
    tft.fillRect(0, 0, 240, 30, TFT_DARKGREY);
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.drawString("HARDWARE DIAGNOSTICS", 10, 8, 2);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("ESP32-S3 Dual-Core 240MHz", 10, 45, 2);
    tft.drawString("Flash: 8MB QIO", 10, 70, 2);
    tft.drawString("PSRAM: Octal SPI OK", 10, 95, 2);
    tft.drawString("Display: ST7789 240x280", 10, 120, 2);
    tft.drawString("Battery: USB-C TP4056", 10, 145, 2);
    tft.drawString("ESP-NOW Mesh: Active", 10, 170, 2);

    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString("Buttons: Test by pressing", 10, 205, 2);

    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("Press B to return to menu", 10, 245, 2);
}

void setup() {
    Serial.begin(115200);

    // Turn on the SPI display
    tft.init();
    tft.setRotation(0); // 240 wide by 280 tall
    tft.fillScreen(TFT_BLACK);

    // Initialize button pins and buzzer
    input.init();
    SoundEngine::init();

    // Setup dock wireless telemetry over ESP-NOW
    dockTelemetry.init();

    drawBootScreen();
}

void loop() {
    input.update();

    switch (currentState) {
        case STATE_BOOT:
            // Any button press exits boot screen
            if (input.anyPressed()) {
                SoundEngine::playSelect();
                currentState = STATE_MENU;
                drawMenu();
            }
            break;

        case STATE_MENU:
            if (input.justPressed(PIN_BTN_UP)) {
                menuSelection--;
                if (menuSelection < 0) menuSelection = MENU_ITEMS_COUNT - 1;
                SoundEngine::playCursorMove();
                drawMenu();
            }
            if (input.justPressed(PIN_BTN_DOWN)) {
                menuSelection++;
                if (menuSelection >= MENU_ITEMS_COUNT) menuSelection = 0;
                SoundEngine::playCursorMove();
                drawMenu();
            }
            if (input.justPressed(PIN_BTN_A)) {
                SoundEngine::playSelect();
                switch (menuSelection) {
                    case 0:
                        currentState = STATE_INVADERS;
                        invadersGame.init(&tft, &input);
                        break;
                    case 1:
                        currentState = STATE_SNAKE;
                        snakeGame.init(&tft, &input);
                        break;
                    case 2:
                        currentState = STATE_BREAKOUT;
                        breakoutGame.init(&tft, &input);
                        break;
                    case 3:
                        currentState = STATE_FLAPPY;
                        flappyGame.init(&tft, &input);
                        break;
                    case 4:
                        currentState = STATE_RACER;
                        racerGame.init(&tft, &input);
                        break;
                    case 5:
                        currentState = STATE_TETRIS;
                        tetrisGame.init(&tft, &input);
                        break;
                    case 6:
                        currentState = STATE_RADAR;
                        cyberRadar.init(&tft, &input);
                        break;
                    case 7:
                        currentState = STATE_DIAGNOSTICS;
                        drawDiagnostics();
                        break;
                }
            }
            break;

        case STATE_INVADERS:
            if (input.justPressed(PIN_BTN_B)) {
                currentState = STATE_MENU;
                drawMenu();
            } else {
                invadersGame.update();
                invadersGame.render();
            }
            break;

        case STATE_SNAKE:
            if (input.justPressed(PIN_BTN_B)) {
                currentState = STATE_MENU;
                drawMenu();
            } else {
                snakeGame.update();
                snakeGame.render();
            }
            break;

        case STATE_BREAKOUT:
            if (input.justPressed(PIN_BTN_B)) {
                currentState = STATE_MENU;
                drawMenu();
            } else {
                breakoutGame.update();
                breakoutGame.render();
            }
            break;

        case STATE_FLAPPY:
            if (input.justPressed(PIN_BTN_B)) {
                currentState = STATE_MENU;
                drawMenu();
            } else {
                flappyGame.update();
                flappyGame.render();
            }
            break;

        case STATE_RACER:
            if (input.justPressed(PIN_BTN_B)) {
                currentState = STATE_MENU;
                drawMenu();
            } else {
                racerGame.update();
                racerGame.render();
            }
            break;

        case STATE_TETRIS:
            if (input.justPressed(PIN_BTN_B)) {
                currentState = STATE_MENU;
                drawMenu();
            } else {
                tetrisGame.update();
                tetrisGame.render();
            }
            break;

        case STATE_RADAR:
            if (input.justPressed(PIN_BTN_B)) {
                currentState = STATE_MENU;
                drawMenu();
            } else {
                cyberRadar.update();
                cyberRadar.render();
            }
            break;

        case STATE_DIAGNOSTICS:
            if (input.justPressed(PIN_BTN_B)) {
                currentState = STATE_MENU;
                drawMenu();
            }
            break;
    }

    // Broadcast periodic battery/pilot telemetry over ESP-NOW
    dockTelemetry.update();

    delay(16); // ~60 fps frame pacing
}
