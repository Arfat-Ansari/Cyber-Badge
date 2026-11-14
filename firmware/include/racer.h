// ==============================================================
// Highway Racer Arcade Game
// Author: Arfat Ansari
// Top-down highway dodging traffic cars with scrolling road
// ==============================================================

#ifndef RACER_H
#define RACER_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "audio.h"
#include "input.h"

#define SCREEN_W 240
#define SCREEN_H 280
#define ROAD_LEFT 35
#define ROAD_RIGHT 205
#define ROAD_WIDTH (ROAD_RIGHT - ROAD_LEFT)

struct TrafficCar {
    float x;
    float y;
    float speed;
    uint16_t color;
    bool active;
};

class ShenzhenRacer {
private:
    TFT_eSPI *tft;
    InputController *input;

    float playerX;
    static const int PLAYER_Y = SCREEN_H - 45;
    static const int CAR_W = 18;
    static const int CAR_H = 30;

    static const int MAX_TRAFFIC = 3;
    TrafficCar traffic[MAX_TRAFFIC];

    float roadScrollY;
    int score;
    bool gameOver;

public:
    void init(TFT_eSPI *display, InputController *in) {
        tft = display;
        input = in;
        reset();
    }

    void reset() {
        playerX = (SCREEN_W - CAR_W) / 2;
        roadScrollY = 0;
        score = 0;
        gameOver = false;

        uint16_t colors[3] = {TFT_RED, TFT_ORANGE, TFT_MAGENTA};
        for (int i = 0; i < MAX_TRAFFIC; i++) {
            traffic[i].x = random(ROAD_LEFT + 8, ROAD_RIGHT - CAR_W - 8);
            traffic[i].y = -random(50, 250) - (i * 100);
            traffic[i].speed = random(20, 35) / 10.0;
            traffic[i].color = colors[i % 3];
            traffic[i].active = true;
        }

        tft->fillScreen(TFT_BLACK);
        drawHUD();
    }

    void drawHUD() {
        tft->fillRect(0, 0, SCREEN_W, 26, TFT_NAVY);
        tft->setTextColor(TFT_CYAN, TFT_NAVY);
        tft->drawString("HIGHWAY RACER", 10, 6, 2);
        tft->setTextColor(TFT_YELLOW, TFT_NAVY);
        tft->drawString("DIST: " + String(score) + "m", 150, 6, 2);
        tft->drawFastHLine(0, 26, SCREEN_W, TFT_DARKGREY);
    }

    void update() {
        if (gameOver) {
            if (input->justPressed(PIN_BTN_A)) reset();
            return;
        }

        // Steer Left / Right
        float steerSpeed = 4.0;
        if (input->isDown(PIN_BTN_LEFT)) playerX -= steerSpeed;
        if (input->isDown(PIN_BTN_RIGHT)) playerX += steerSpeed;

        if (playerX < ROAD_LEFT + 4) playerX = ROAD_LEFT + 4;
        if (playerX > ROAD_RIGHT - CAR_W - 4) playerX = ROAD_RIGHT - CAR_W - 4;

        // Turbo Speed Boost on Button A
        float gameSpeed = input->isDown(PIN_BTN_A) ? 7.0 : 4.0;
        roadScrollY += gameSpeed;
        if (roadScrollY >= 40) roadScrollY = 0;

        score += (int)gameSpeed;
        if (score % 50 == 0) drawHUD();

        // Update Traffic
        for (int i = 0; i < MAX_TRAFFIC; i++) {
            traffic[i].y += (gameSpeed - traffic[i].speed);

            // Recycle traffic cars
            if (traffic[i].y > SCREEN_H) {
                traffic[i].y = -random(40, 100);
                traffic[i].x = random(ROAD_LEFT + 8, ROAD_RIGHT - CAR_W - 8);
                traffic[i].speed = random(18, 32) / 10.0;
            }

            // Hitbox Collision
            if (playerX < traffic[i].x + CAR_W && playerX + CAR_W > traffic[i].x &&
                PLAYER_Y < traffic[i].y + CAR_H && PLAYER_Y + CAR_H > traffic[i].y) {
                gameOver = true;
                SoundEngine::playExplosion();
                return;
            }
        }
    }

    void drawCar(int x, int y, uint16_t color) {
        // Main Car Body
        tft->fillRoundRect(x, y, CAR_W, CAR_H, 3, color);
        // Windshield
        tft->fillRect(x + 2, y + 8, CAR_W - 4, 6, TFT_BLACK);
        // Wheels
        tft->fillRect(x - 2, y + 4, 3, 6, TFT_DARKGREY);
        tft->fillRect(x + CAR_W - 1, y + 4, 3, 6, TFT_DARKGREY);
        tft->fillRect(x - 2, y + CAR_H - 10, 3, 6, TFT_DARKGREY);
        tft->fillRect(x + CAR_W - 1, y + CAR_H - 10, 3, 6, TFT_DARKGREY);
    }

    void render() {
        // Clear Grass/Sidewalk
        tft->fillRect(0, 27, ROAD_LEFT, SCREEN_H - 27, TFT_DARKGREEN);
        tft->fillRect(ROAD_RIGHT, 27, SCREEN_W - ROAD_RIGHT, SCREEN_H - 27, TFT_DARKGREEN);

        // Clear Asphalt Road
        tft->fillRect(ROAD_LEFT, 27, ROAD_WIDTH, SCREEN_H - 27, TFT_BLACK);

        if (gameOver) {
            tft->setTextColor(TFT_RED, TFT_BLACK);
            tft->drawCentreString("WRECKED!", SCREEN_W / 2, 120, 4);
            tft->setTextColor(TFT_YELLOW, TFT_BLACK);
            tft->drawCentreString("TOTAL DISTANCE: " + String(score) + "m", SCREEN_W / 2, 160, 2);
            tft->setTextColor(TFT_WHITE, TFT_BLACK);
            tft->drawCentreString("PRESS A TO RETRY", SCREEN_W / 2, 190, 2);
            return;
        }

        // Draw Road Guardrails
        tft->drawFastVLine(ROAD_LEFT, 27, SCREEN_H - 27, TFT_RED);
        tft->drawFastVLine(ROAD_LEFT + 2, 27, SCREEN_H - 27, TFT_WHITE);
        tft->drawFastVLine(ROAD_RIGHT - 2, 27, SCREEN_H - 27, TFT_WHITE);
        tft->drawFastVLine(ROAD_RIGHT, 27, SCREEN_H - 27, TFT_RED);

        // Draw Center Dashed Line
        int centerX = ROAD_LEFT + (ROAD_WIDTH / 2);
        for (int y = -20 + (int)roadScrollY; y < SCREEN_H; y += 40) {
            if (y >= 27) {
                tft->fillRect(centerX - 1, y, 3, 20, TFT_YELLOW);
            }
        }

        // Draw Traffic Cars
        for (int i = 0; i < MAX_TRAFFIC; i++) {
            if (traffic[i].y >= 20 && traffic[i].y < SCREEN_H) {
                drawCar(traffic[i].x, traffic[i].y, traffic[i].color);
            }
        }

        // Draw Player Starship / Cyber Car (Cyan)
        drawCar(playerX, PLAYER_Y, TFT_CYAN);
    }
};

#endif // RACER_H
