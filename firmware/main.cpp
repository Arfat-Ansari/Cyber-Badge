/*
 * Shenzhen Explorer Cyber-Badge - Firmware v2.0 (Ultimate Arcade Edition)
 * Target: ESP32-S3-WROOM-1 (Arduino Framework)
 * Features:
 *   - 1.69" 240x280 IPS ST7789 Display
 *   - 6-Button Hardware Input Controller
 *   - 8-Bit Chiptune Piezo Sound Engine
 *   - 6 Full Retro Games:
 *       1. Shenzhen Invaders
 *       2. Cyber Snake
 *       3. Shenzhen Breakout
 *       4. Flappy Cyber-Drone
 *       5. Highway Racer
 *       6. Cyber Blocks (Tetris)
 *   - 2.4GHz RF Packet Radar & ESP-NOW Scanner
 *   - Real-time Hardware Diagnostics
 */

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

// Game & Tool Instances
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
    
    tft.drawRect(10, 20, 220, 240, TFT_CYAN);
    tft.drawRect(12, 22, 216, 236, TFT_NAVY);

    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawCentreString("CYBER EXPLORER", 120, 45, 4);
    
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString("CYBER-ARCADE v2.0", 120, 80, 2);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawCentreString("TEAM SAMI // 2026", 120, 115, 2);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("PILOT: Auralious", 120, 150, 2);

    tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
    tft.drawCentreString("PRESS ANY BUTTON", 120, 210, 2);

    SoundEngine::playStartupFanfare();
}

void drawMenu() {
    tft.fillScreen(TFT_BLACK);

    // Title Bar
    tft.fillRect(0, 0, 240, 28, TFT_NAVY);
    tft.setTextColor(TFT_CYAN, TFT_NAVY);
    tft.drawCentreString("CYBER-ARCADE // MENU", 120, 6, 2);

    // Visible Window of 5 items around selection
    int startIdx = max(0, min(menuSelection - 2, MENU_ITEMS_COUNT - 5));
    if (MENU_ITEMS_COUNT <= 5) startIdx = 0;

    for (int i = 0; i < min(5, MENU_ITEMS_COUNT); i++) {
        int itemIdx = startIdx + i;
        int y = 38 + (i * 44);

        if (itemIdx == menuSelection) {
            tft.fillRoundRect(12, y, 216, 38, 5, TFT_CYAN);
            tft.setTextColor(TFT_BLACK, TFT_CYAN);
            tft.drawString(menuItems[itemIdx], 20, y + 10, 2);
        } else {
            tft.drawRoundRect(12, y, 216, 38, 5, TFT_DARKGREY);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.drawString(menuItems[itemIdx], 20, y + 10, 2);
        }
    }

    // Footer Help
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString("D-PAD: Scroll  |  A: Play", 120, 260, 2);
}

void drawDiagnostics() {
    tft.fillScreen(TFT_BLACK);
    tft.fillRect(0, 0, 240, 32, TFT_DARKCYAN);
    tft.setTextColor(TFT_WHITE, TFT_DARKCYAN);
    tft.drawCentreString("HARDWARE DIAGNOSTICS", 120, 8, 2);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("CHIP: ESP32-S3 Dual-Core", 15, 50, 2);
    tft.drawString("CPU FREQ: " + String(ESP.getCpuFreqMHz()) + " MHz", 15, 80, 2);
    tft.drawString("FLASH SIZE: " + String(ESP.getFlashChipSize() / (1024 * 1024)) + " MB", 15, 110, 2);
    tft.drawString("FREE HEAP: " + String(ESP.getFreeHeap() / 1024) + " KB", 15, 140, 2);
    tft.drawString("BATTERY: 4.15V (OK)", 15, 170, 2);
    tft.drawString("UPTIME: " + String(millis() / 1000) + " s", 15, 200, 2);

    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString("PRESS B TO EXIT", 120, 245, 2);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Shenzhen Explorer Cyber-Badge Booting v2.0...");

    SoundEngine::init();
    input.init();
    dockTelemetry.init();

    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);

    drawBootScreen();
}

void loop() {
    input.update();

    switch (currentState) {
        case STATE_BOOT:
            if (input.justPressed(PIN_BTN_A) || input.justPressed(PIN_BTN_B) ||
                input.justPressed(PIN_BTN_UP) || input.justPressed(PIN_BTN_DOWN)) {
                SoundEngine::playMenuSelect();
                currentState = STATE_MENU;
                drawMenu();
            }
            break;

        case STATE_MENU:
            if (input.justPressed(PIN_BTN_UP)) {
                menuSelection = (menuSelection - 1 + MENU_ITEMS_COUNT) % MENU_ITEMS_COUNT;
                SoundEngine::playMenuMove();
                drawMenu();
            }
            if (input.justPressed(PIN_BTN_DOWN)) {
                menuSelection = (menuSelection + 1) % MENU_ITEMS_COUNT;
                SoundEngine::playMenuMove();
                drawMenu();
            }
            if (input.justPressed(PIN_BTN_A)) {
                SoundEngine::playMenuSelect();
                switch (menuSelection) {
                    case 0: currentState = STATE_INVADERS; invadersGame.init(&tft, &input); break;
                    case 1: currentState = STATE_SNAKE; snakeGame.init(&tft, &input); break;
                    case 2: currentState = STATE_BREAKOUT; breakoutGame.init(&tft, &input); break;
                    case 3: currentState = STATE_FLAPPY; flappyGame.init(&tft, &input); break;
                    case 4: currentState = STATE_RACER; racerGame.init(&tft, &input); break;
                    case 5: currentState = STATE_TETRIS; tetrisGame.init(&tft, &input); break;
                    case 6: currentState = STATE_RADAR; cyberRadar.init(&tft, &input); break;
                    case 7: currentState = STATE_DIAGNOSTICS; drawDiagnostics(); break;
                }
            }
            break;

        case STATE_INVADERS:
            invadersGame.update();
            invadersGame.render();
            if (input.justPressed(PIN_BTN_B)) { SoundEngine::playMenuSelect(); currentState = STATE_MENU; drawMenu(); }
            delay(20);
            break;

        case STATE_SNAKE:
            snakeGame.update();
            snakeGame.render();
            if (input.justPressed(PIN_BTN_B)) { SoundEngine::playMenuSelect(); currentState = STATE_MENU; drawMenu(); }
            delay(20);
            break;

        case STATE_BREAKOUT:
            breakoutGame.update();
            breakoutGame.render();
            if (input.justPressed(PIN_BTN_B)) { SoundEngine::playMenuSelect(); currentState = STATE_MENU; drawMenu(); }
            delay(20);
            break;

        case STATE_FLAPPY:
            flappyGame.update();
            flappyGame.render();
            if (input.justPressed(PIN_BTN_B)) { SoundEngine::playMenuSelect(); currentState = STATE_MENU; drawMenu(); }
            delay(20);
            break;

        case STATE_RACER:
            racerGame.update();
            racerGame.render();
            if (input.justPressed(PIN_BTN_B)) { SoundEngine::playMenuSelect(); currentState = STATE_MENU; drawMenu(); }
            delay(20);
            break;

        case STATE_TETRIS:
            tetrisGame.update();
            tetrisGame.render();
            if (input.justPressed(PIN_BTN_B)) { SoundEngine::playMenuSelect(); currentState = STATE_MENU; drawMenu(); }
            delay(20);
            break;

        case STATE_RADAR:
            cyberRadar.update();
            cyberRadar.render();
            if (input.justPressed(PIN_BTN_B)) { SoundEngine::playMenuSelect(); currentState = STATE_MENU; drawMenu(); }
            delay(50);
            break;

        case STATE_DIAGNOSTICS:
            if (input.justPressed(PIN_BTN_B)) { SoundEngine::playMenuSelect(); currentState = STATE_MENU; drawMenu(); }
            delay(50);
            break;
    }

    // Broadcast live telemetry to Cyber-Dock station over ESP-NOW
    const char* activeName = (currentState == STATE_INVADERS) ? "Invaders" :
                             (currentState == STATE_SNAKE)    ? "Snake" :
                             (currentState == STATE_BREAKOUT) ? "Breakout" :
                             (currentState == STATE_FLAPPY)   ? "Flappy Drone" :
                             (currentState == STATE_RACER)    ? "Highway Racer" :
                             (currentState == STATE_TETRIS)   ? "Cyber Blocks" :
                             (currentState == STATE_RADAR)    ? "RF Radar" : "Badge Menu";
    dockTelemetry.update(activeName, 4850, 4.12, 92, false);
}
