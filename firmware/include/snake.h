#ifndef SNAKE_H
#define SNAKE_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "audio.h"
#include "input.h"

#define SCREEN_W 240
#define SCREEN_H 280
#define GRID_SIZE 12
#define GRID_W (SCREEN_W / GRID_SIZE)
#define GRID_H ((SCREEN_H - 30) / GRID_SIZE)

struct Point {
    int x;
    int y;
};

class CyberSnake {
private:
    TFT_eSPI *tft;
    InputController *input;

    Point snake[150];
    int snakeLength;
    int dirX, dirY;
    Point food;
    int score;
    bool gameOver;
    unsigned long lastStep;
    int stepDelay;

    void spawnFood() {
        bool valid = false;
        while (!valid) {
            food.x = random(1, GRID_W - 1);
            food.y = random(1, GRID_H - 1);
            valid = true;
            for (int i = 0; i < snakeLength; i++) {
                if (snake[i].x == food.x && snake[i].y == food.y) {
                    valid = false;
                    break;
                }
            }
        }
    }

public:
    void init(TFT_eSPI *display, InputController *in) {
        tft = display;
        input = in;
        reset();
    }

    void reset() {
        snakeLength = 4;
        dirX = 1;
        dirY = 0;
        score = 0;
        gameOver = false;
        stepDelay = 120;
        lastStep = millis();

        int startX = GRID_W / 2;
        int startY = GRID_H / 2;
        for (int i = 0; i < snakeLength; i++) {
            snake[i].x = startX - i;
            snake[i].y = startY;
        }

        spawnFood();
        tft->fillScreen(TFT_BLACK);
        drawHUD();
    }

    void drawHUD() {
        tft->fillRect(0, 0, SCREEN_W, 26, TFT_NAVY);
        tft->setTextColor(TFT_CYAN, TFT_NAVY);
        tft->drawString("CYBER SNAKE", 10, 6, 2);
        tft->setTextColor(TFT_YELLOW, TFT_NAVY);
        tft->drawString("SCORE: " + String(score), 150, 6, 2);
        tft->drawFastHLine(0, 26, SCREEN_W, TFT_DARKGREY);
    }

    void update() {
        if (gameOver) {
            if (input->justPressed(PIN_BTN_A)) reset();
            return;
        }

        // D-Pad Direction changes
        if (input->justPressed(PIN_BTN_UP) && dirY == 0) { dirX = 0; dirY = -1; }
        if (input->justPressed(PIN_BTN_DOWN) && dirY == 0) { dirX = 0; dirY = 1; }
        if (input->justPressed(PIN_BTN_LEFT) && dirX == 0) { dirX = -1; dirY = 0; }
        if (input->justPressed(PIN_BTN_RIGHT) && dirX == 0) { dirX = 1; dirY = 0; }

        // Turbo Speed Boost with Button A
        int currentDelay = input->isDown(PIN_BTN_A) ? 50 : stepDelay;

        if (millis() - lastStep > currentDelay) {
            lastStep = millis();

            // Next Head Position
            int nextX = snake[0].x + dirX;
            int nextY = snake[0].y + dirY;

            // Wall Collisions
            if (nextX < 0 || nextX >= GRID_W || nextY < 0 || nextY >= GRID_H) {
                gameOver = true;
                SoundEngine::playExplosion();
                return;
            }

            // Self Collision
            for (int i = 0; i < snakeLength; i++) {
                if (snake[i].x == nextX && snake[i].y == nextY) {
                    gameOver = true;
                    SoundEngine::playExplosion();
                    return;
                }
            }

            // Check Food Eaten
            bool ateFood = (nextX == food.x && nextY == food.y);

            // Shift Body
            for (int i = snakeLength; i > 0; i--) {
                snake[i] = snake[i - 1];
            }
            snake[0].x = nextX;
            snake[0].y = nextY;

            if (ateFood) {
                if (snakeLength < 145) snakeLength++;
                score += 50;
                if (stepDelay > 60) stepDelay -= 2; // Speeds up gradually
                SoundEngine::playMenuSelect();
                spawnFood();
                drawHUD();
            }
        }
    }

    void render() {
        tft->fillRect(0, 27, SCREEN_W, SCREEN_H - 27, TFT_BLACK);

        if (gameOver) {
            tft->setTextColor(TFT_RED, TFT_BLACK);
            tft->drawCentreString("GAME OVER", SCREEN_W / 2, 120, 4);
            tft->setTextColor(TFT_YELLOW, TFT_BLACK);
            tft->drawCentreString("FINAL SCORE: " + String(score), SCREEN_W / 2, 160, 2);
            tft->setTextColor(TFT_WHITE, TFT_BLACK);
            tft->drawCentreString("PRESS A TO RETRY", SCREEN_W / 2, 190, 2);
            return;
        }

        // Draw Food (Pulsing Amber Power Pellet)
        int fx = food.x * GRID_SIZE;
        int fy = 28 + food.y * GRID_SIZE;
        tft->fillCircle(fx + GRID_SIZE / 2, fy + GRID_SIZE / 2, (GRID_SIZE / 2) - 1, TFT_YELLOW);

        // Draw Snake Body (Neon Cyan to Green)
        for (int i = 0; i < snakeLength; i++) {
            int sx = snake[i].x * GRID_SIZE;
            int sy = 28 + snake[i].y * GRID_SIZE;
            uint16_t color = (i == 0) ? TFT_CYAN : TFT_GREEN;
            tft->fillRoundRect(sx + 1, sy + 1, GRID_SIZE - 2, GRID_SIZE - 2, 2, color);
        }
    }
};

#endif // SNAKE_H
