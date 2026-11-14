// ==============================================================
// Shenzhen Breakout Arcade Game
// Author: Arfat Ansari
// Paddle brick breaker with 2-row chip blocks and sound effects
// ==============================================================

#ifndef BREAKOUT_H
#define BREAKOUT_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "audio.h"
#include "input.h"

#define SCREEN_W 240
#define SCREEN_H 280

struct Brick {
    int x;
    int y;
    bool alive;
    uint16_t color;
};

class ShenzhenBreakout {
private:
    TFT_eSPI *tft;
    InputController *input;

    float paddleX;
    static const int PADDLE_W = 44;
    static const int PADDLE_H = 7;
    static const int PADDLE_Y = SCREEN_H - 22;

    float ballX, ballY;
    float ballDX, ballDY;
    static const int BALL_R = 3;
    bool ballLaunched;

    static const int ROWS = 4;
    static const int COLS = 6;
    Brick bricks[ROWS * COLS];

    int score;
    int lives;
    bool gameOver;
    bool victory;

public:
    void init(TFT_eSPI *display, InputController *in) {
        tft = display;
        input = in;
        reset();
    }

    void reset() {
        paddleX = (SCREEN_W - PADDLE_W) / 2;
        ballX = SCREEN_W / 2;
        ballY = PADDLE_Y - 8;
        ballDX = 2.5;
        ballDY = -2.5;
        ballLaunched = false;
        score = 0;
        lives = 3;
        gameOver = false;
        victory = false;

        uint16_t rowColors[ROWS] = {TFT_RED, TFT_ORANGE, TFT_GREEN, TFT_CYAN};
        int idx = 0;
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                bricks[idx].x = 10 + c * 37;
                bricks[idx].y = 40 + r * 16;
                bricks[idx].alive = true;
                bricks[idx].color = rowColors[r];
                idx++;
            }
        }

        tft->fillScreen(TFT_BLACK);
        drawHUD();
    }

    void drawHUD() {
        tft->fillRect(0, 0, SCREEN_W, 26, TFT_NAVY);
        tft->setTextColor(TFT_CYAN, TFT_NAVY);
        tft->drawString("CYBER BREAKOUT", 10, 6, 2);
        tft->setTextColor(TFT_YELLOW, TFT_NAVY);
        tft->drawString("LIVES: " + String(lives) + "  SCORE: " + String(score), 100, 6, 2);
        tft->drawFastHLine(0, 26, SCREEN_W, TFT_DARKGREY);
    }

    void update() {
        if (gameOver || victory) {
            if (input->justPressed(PIN_BTN_A)) reset();
            return;
        }

        // Paddle Movement (D-Pad Left/Right)
        if (input->isDown(PIN_BTN_LEFT)) paddleX -= 4.5;
        if (input->isDown(PIN_BTN_RIGHT)) paddleX += 4.5;
        if (paddleX < 5) paddleX = 5;
        if (paddleX > SCREEN_W - PADDLE_W - 5) paddleX = SCREEN_W - PADDLE_W - 5;

        // Launch ball on Button A
        if (!ballLaunched) {
            ballX = paddleX + (PADDLE_W / 2);
            ballY = PADDLE_Y - 8;
            if (input->justPressed(PIN_BTN_A)) {
                ballLaunched = true;
                SoundEngine::playLaser();
            }
            return;
        }

        // Ball Physics
        ballX += ballDX;
        ballY += ballDY;

        // Wall Collisions
        if (ballX <= BALL_R || ballX >= SCREEN_W - BALL_R) {
            ballDX = -ballDX;
            SoundEngine::playTone(600, 15);
        }
        if (ballY <= 28 + BALL_R) {
            ballDY = -ballDY;
            SoundEngine::playTone(600, 15);
        }

        // Paddle Collision
        if (ballY >= PADDLE_Y - BALL_R && ballY <= PADDLE_Y + PADDLE_H &&
            ballX >= paddleX - 2 && ballX <= paddleX + PADDLE_W + 2) {
            ballDY = -abs(ballDY);
            // Angle variation based on where it hits the paddle
            float hitOffset = (ballX - (paddleX + PADDLE_W / 2.0)) / (PADDLE_W / 2.0);
            ballDX = hitOffset * 3.5;
            SoundEngine::playTone(850, 20);
        }

        // Brick Collisions
        bool anyBricksLeft = false;
        for (int i = 0; i < ROWS * COLS; i++) {
            if (bricks[i].alive) {
                anyBricksLeft = true;
                if (ballX >= bricks[i].x - BALL_R && ballX <= bricks[i].x + 34 + BALL_R &&
                    ballY >= bricks[i].y - BALL_R && ballY <= bricks[i].y + 12 + BALL_R) {
                    bricks[i].alive = false;
                    ballDY = -ballDY;
                    score += 100;
                    SoundEngine::playExplosion();
                    drawHUD();
                    break;
                }
            }
        }
        if (!anyBricksLeft) victory = true;

        // Ball Fell Off Bottom
        if (ballY > SCREEN_H) {
            lives--;
            SoundEngine::playExplosion();
            drawHUD();
            if (lives <= 0) {
                gameOver = true;
            } else {
                ballLaunched = false;
            }
        }
    }

    void render() {
        tft->fillRect(0, 27, SCREEN_W, SCREEN_H - 27, TFT_BLACK);

        if (gameOver) {
            tft->setTextColor(TFT_RED, TFT_BLACK);
            tft->drawCentreString("GAME OVER", SCREEN_W / 2, 120, 4);
            tft->setTextColor(TFT_WHITE, TFT_BLACK);
            tft->drawCentreString("PRESS A TO RETRY", SCREEN_W / 2, 160, 2);
            return;
        }

        if (victory) {
            tft->setTextColor(TFT_CYAN, TFT_BLACK);
            tft->drawCentreString("BRICKS SMASHED!", SCREEN_W / 2, 120, 4);
            tft->setTextColor(TFT_YELLOW, TFT_BLACK);
            tft->drawCentreString("SCORE: " + String(score), SCREEN_W / 2, 160, 2);
            return;
        }

        // Draw Bricks
        for (int i = 0; i < ROWS * COLS; i++) {
            if (bricks[i].alive) {
                tft->fillRoundRect(bricks[i].x, bricks[i].y, 34, 12, 2, bricks[i].color);
                tft->drawRoundRect(bricks[i].x, bricks[i].y, 34, 12, 2, TFT_WHITE);
            }
        }

        // Draw Paddle
        tft->fillRoundRect(paddleX, PADDLE_Y, PADDLE_W, PADDLE_H, 3, TFT_CYAN);

        // Draw Ball
        tft->fillCircle(ballX, ballY, BALL_R, TFT_YELLOW);
    }
};

#endif // BREAKOUT_H
