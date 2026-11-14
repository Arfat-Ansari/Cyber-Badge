// ==============================================================
// Flappy Cyber-Drone Arcade Game
// Author: Arfat Ansari
// Single-button physics flight tapping through neon obstacles
// ==============================================================

#ifndef FLAPPY_H
#define FLAPPY_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "audio.h"
#include "input.h"

#define SCREEN_W 240
#define SCREEN_H 280

struct Pipe {
    float x;
    int gapY;
    bool passed;
};

class FlappyDrone {
private:
    TFT_eSPI *tft;
    InputController *input;

    float droneY;
    float droneVelocity;
    static const int DRONE_X = 50;
    static const int DRONE_R = 7;
    const float GRAVITY = 0.38;
    const float JUMP_FORCE = -5.8;

    static const int PIPE_COUNT = 2;
    Pipe pipes[PIPE_COUNT];
    static const int PIPE_GAP = 68;
    static const int PIPE_WIDTH = 28;
    float pipeSpeed;

    int score;
    bool gameOver;

public:
    void init(TFT_eSPI *display, InputController *in) {
        tft = display;
        input = in;
        reset();
    }

    void reset() {
        droneY = 130;
        droneVelocity = 0;
        score = 0;
        gameOver = false;
        pipeSpeed = 2.4;

        for (int i = 0; i < PIPE_COUNT; i++) {
            pipes[i].x = SCREEN_W + (i * 150);
            pipes[i].gapY = random(60, 180);
            pipes[i].passed = false;
        }

        tft->fillScreen(TFT_BLACK);
        drawHUD();
    }

    void drawHUD() {
        tft->fillRect(0, 0, SCREEN_W, 26, TFT_NAVY);
        tft->setTextColor(TFT_CYAN, TFT_NAVY);
        tft->drawString("CYBER DRONE", 10, 6, 2);
        tft->setTextColor(TFT_YELLOW, TFT_NAVY);
        tft->drawString("GATES: " + String(score), 150, 6, 2);
        tft->drawFastHLine(0, 26, SCREEN_W, TFT_DARKGREY);
    }

    void update() {
        if (gameOver) {
            if (input->justPressed(PIN_BTN_A)) reset();
            return;
        }

        // Thrust upward on Button A or D-Pad UP
        if (input->justPressed(PIN_BTN_A) || input->justPressed(PIN_BTN_UP)) {
            droneVelocity = JUMP_FORCE;
            SoundEngine::playTone(950, 15);
        }

        // Physics
        droneVelocity += GRAVITY;
        droneY += droneVelocity;

        // Ground / Ceiling Collision
        if (droneY <= 28 + DRONE_R || droneY >= SCREEN_H - DRONE_R) {
            gameOver = true;
            SoundEngine::playExplosion();
            return;
        }

        // Move Pipes
        for (int i = 0; i < PIPE_COUNT; i++) {
            pipes[i].x -= pipeSpeed;

            // Pipe Passed
            if (!pipes[i].passed && pipes[i].x + PIPE_WIDTH < DRONE_X) {
                pipes[i].passed = true;
                score++;
                SoundEngine::playMenuSelect();
                drawHUD();
            }

            // Pipe Recycle
            if (pipes[i].x < -PIPE_WIDTH) {
                pipes[i].x = SCREEN_W + 50;
                pipes[i].gapY = random(60, 180);
                pipes[i].passed = false;
            }

            // Pipe Collision Check
            if (DRONE_X + DRONE_R >= pipes[i].x && DRONE_X - DRONE_R <= pipes[i].x + PIPE_WIDTH) {
                if (droneY - DRONE_R <= pipes[i].gapY || droneY + DRONE_R >= pipes[i].gapY + PIPE_GAP) {
                    gameOver = true;
                    SoundEngine::playExplosion();
                    return;
                }
            }
        }
    }

    void render() {
        tft->fillRect(0, 27, SCREEN_W, SCREEN_H - 27, TFT_BLACK);

        if (gameOver) {
            tft->setTextColor(TFT_RED, TFT_BLACK);
            tft->drawCentreString("CRASHED!", SCREEN_W / 2, 120, 4);
            tft->setTextColor(TFT_YELLOW, TFT_BLACK);
            tft->drawCentreString("GATES PASSED: " + String(score), SCREEN_W / 2, 160, 2);
            tft->setTextColor(TFT_WHITE, TFT_BLACK);
            tft->drawCentreString("PRESS A TO RETRY", SCREEN_W / 2, 190, 2);
            return;
        }

        // Draw Laser Gate Pipes (Neon Magenta Pillars)
        for (int i = 0; i < PIPE_COUNT; i++) {
            // Top pillar
            tft->fillRect(pipes[i].x, 27, PIPE_WIDTH, pipes[i].gapY - 27, TFT_MAGENTA);
            tft->drawRect(pipes[i].x, 27, PIPE_WIDTH, pipes[i].gapY - 27, TFT_WHITE);

            // Bottom pillar
            int botY = pipes[i].gapY + PIPE_GAP;
            tft->fillRect(pipes[i].x, botY, PIPE_WIDTH, SCREEN_H - botY, TFT_MAGENTA);
            tft->drawRect(pipes[i].x, botY, PIPE_WIDTH, SCREEN_H - botY, TFT_WHITE);
        }

        // Draw Cyber Drone (Glowing Cyan Disk with Yellow Core)
        tft->fillCircle(DRONE_X, droneY, DRONE_R, TFT_CYAN);
        tft->fillCircle(DRONE_X, droneY, 3, TFT_YELLOW);
        // Thruster flame when going up
        if (droneVelocity < 0) {
            tft->fillTriangle(DRONE_X - DRONE_R, droneY + 2, DRONE_X - DRONE_R - 5, droneY, DRONE_X - DRONE_R, droneY - 2, TFT_ORANGE);
        }
    }
};

#endif // FLAPPY_H
