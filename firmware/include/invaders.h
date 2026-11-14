// ==============================================================
// Shenzhen Invaders Arcade Game
// Author: Arfat Ansari
// Classic space shooter written for the 1.69" IPS screen
// ==============================================================

#ifndef INVADERS_H
#define INVADERS_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "audio.h"
#include "input.h"

#define SCREEN_W 240
#define SCREEN_H 280

struct Bullet {
    int x;
    int y;
    bool active;
};

struct Alien {
    int x;
    int y;
    bool alive;
};

class ShenzhenInvaders {
private:
    TFT_eSPI *tft;
    InputController *input;

    int playerX;
    int playerY;
    static const int PLAYER_W = 20;
    static const int PLAYER_H = 10;
    static const int PLAYER_SPEED = 4;

    Bullet bullet;
    static const int MAX_ALIENS = 12;
    Alien aliens[MAX_ALIENS];
    int alienDirection;
    int alienSpeed;
    unsigned long lastAlienMove;

    int score;
    bool gameOver;
    bool victory;

public:
    void init(TFT_eSPI *display, InputController *in) {
        tft = display;
        input = in;
        reset();
    }

    void reset() {
        playerX = (SCREEN_W - PLAYER_W) / 2;
        playerY = SCREEN_H - 30;

        bullet.active = false;

        // Initialize 2 rows of 6 aliens
        int index = 0;
        for (int row = 0; row < 2; row++) {
            for (int col = 0; col < 6; col++) {
                aliens[index].x = 30 + col * 32;
                aliens[index].y = 40 + row * 24;
                aliens[index].alive = true;
                index++;
            }
        }

        alienDirection = 1;
        alienSpeed = 2;
        lastAlienMove = millis();
        score = 0;
        gameOver = false;
        victory = false;

        tft->fillScreen(TFT_BLACK);
        drawHUD();
    }

    void drawHUD() {
        tft->setTextColor(TFT_GREEN, TFT_BLACK);
        tft->setTextSize(1);
        tft->drawString("CYBER INVADERS", 10, 8, 2);
        tft->setTextColor(TFT_YELLOW, TFT_BLACK);
        tft->drawString("SCORE: " + String(score), 150, 8, 2);
        tft->drawFastHLine(0, 26, SCREEN_W, TFT_DARKGREY);
    }

    void update() {
        if (gameOver || victory) {
            if (input->justPressed(PIN_BTN_A)) {
                reset();
            }
            return;
        }

        // 1. Player Movement (D-Pad Left / Right)
        if (input->isDown(PIN_BTN_LEFT)) {
            playerX -= PLAYER_SPEED;
            if (playerX < 0) playerX = 0;
        }
        if (input->isDown(PIN_BTN_RIGHT)) {
            playerX += PLAYER_SPEED;
            if (playerX > SCREEN_W - PLAYER_W) playerX = SCREEN_W - PLAYER_W;
        }

        // 2. Player Shooting (Button A)
        if (input->justPressed(PIN_BTN_A) && !bullet.active) {
            bullet.x = playerX + (PLAYER_W / 2);
            bullet.y = playerY - 4;
            bullet.active = true;
            SoundEngine::playLaser();
        }

        // 3. Update Bullet
        if (bullet.active) {
            bullet.y -= 7;
            if (bullet.y < 28) {
                bullet.active = false;
            } else {
                // Check Collision with Aliens
                for (int i = 0; i < MAX_ALIENS; i++) {
                    if (aliens[i].alive) {
                        if (bullet.x >= aliens[i].x && bullet.x <= aliens[i].x + 16 &&
                            bullet.y >= aliens[i].y && bullet.y <= aliens[i].y + 12) {
                            aliens[i].alive = false;
                            bullet.active = false;
                            score += 100;
                            SoundEngine::playExplosion();
                            drawHUD();
                            break;
                        }
                    }
                }
            }
        }

        // 4. Update Aliens March
        if (millis() - lastAlienMove > 250) {
            lastAlienMove = millis();
            bool hitEdge = false;

            for (int i = 0; i < MAX_ALIENS; i++) {
                if (aliens[i].alive) {
                    aliens[i].x += (alienDirection * 4);
                    if (aliens[i].x <= 10 || aliens[i].x >= SCREEN_W - 26) {
                        hitEdge = true;
                    }
                    // Check if reached player level
                    if (aliens[i].y >= playerY - 10) {
                        gameOver = true;
                    }
                }
            }

            if (hitEdge) {
                alienDirection = -alienDirection;
                for (int i = 0; i < MAX_ALIENS; i++) {
                    if (aliens[i].alive) {
                        aliens[i].y += 10;
                    }
                }
            }

            // Check Victory
            bool anyAlive = false;
            for (int i = 0; i < MAX_ALIENS; i++) {
                if (aliens[i].alive) anyAlive = true;
            }
            if (!anyAlive) victory = true;
        }
    }

    void render() {
        // Clear Playfield area
        tft->fillRect(0, 28, SCREEN_W, SCREEN_H - 28, TFT_BLACK);

        if (gameOver) {
            tft->setTextColor(TFT_RED, TFT_BLACK);
            tft->drawCentreString("GAME OVER", SCREEN_W / 2, 120, 4);
            tft->setTextColor(TFT_WHITE, TFT_BLACK);
            tft->drawCentreString("PRESS A TO RETRY", SCREEN_W / 2, 160, 2);
            return;
        }

        if (victory) {
            tft->setTextColor(TFT_CYAN, TFT_BLACK);
            tft->drawCentreString("SECTOR CLEARED!", SCREEN_W / 2, 120, 4);
            tft->setTextColor(TFT_YELLOW, TFT_BLACK);
            tft->drawCentreString("FINAL SCORE: " + String(score), SCREEN_W / 2, 160, 2);
            tft->setTextColor(TFT_WHITE, TFT_BLACK);
            tft->drawCentreString("PRESS A TO PLAY AGAIN", SCREEN_W / 2, 190, 2);
            return;
        }

        // Draw Player Ship (Cyan Space Fighter)
        tft->fillTriangle(playerX, playerY + PLAYER_H, 
                          playerX + PLAYER_W, playerY + PLAYER_H, 
                          playerX + (PLAYER_W / 2), playerY, TFT_CYAN);
        tft->fillRect(playerX + (PLAYER_W / 2) - 2, playerY + 3, 4, 7, TFT_WHITE);

        // Draw Bullet (Laser line)
        if (bullet.active) {
            tft->drawLine(bullet.x, bullet.y, bullet.x, bullet.y - 6, TFT_YELLOW);
        }

        // Draw Aliens (Retro 8-Bit Blocks)
        for (int i = 0; i < MAX_ALIENS; i++) {
            if (aliens[i].alive) {
                uint16_t color = (i < 6) ? TFT_MAGENTA : TFT_GREEN;
                tft->fillRect(aliens[i].x, aliens[i].y, 16, 12, color);
                tft->drawPixel(aliens[i].x + 3, aliens[i].y + 3, TFT_BLACK);
                tft->drawPixel(aliens[i].x + 11, aliens[i].y + 3, TFT_BLACK);
            }
        }
    }
};

#endif // INVADERS_H
