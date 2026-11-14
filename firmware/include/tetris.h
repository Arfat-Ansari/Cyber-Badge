// ==============================================================
// Cyber Blocks (Tetris) Arcade Game
// Author: Arfat Ansari
// Falling tetromino matrix with rotation and line clears
// ==============================================================

#ifndef TETRIS_H
#define TETRIS_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "audio.h"
#include "input.h"

#define TETRIS_COLS 10
#define TETRIS_ROWS 18
#define BLOCK_SIZE 12
#define BOARD_OFFSET_X 60
#define BOARD_OFFSET_Y 45

// 7 Tetromino Shapes in 4x4 bounding boxes
const uint8_t SHAPES[7][4][4] = {
    // I
    {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}},
    // J
    {{1,0,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
    // L
    {{0,0,1,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
    // O
    {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
    // S
    {{0,1,1,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}},
    // T
    {{0,1,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
    // Z
    {{1,1,0,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}
};

const uint16_t SHAPE_COLORS[7] = {
    TFT_CYAN, TFT_BLUE, TFT_ORANGE, TFT_YELLOW, TFT_GREEN, TFT_MAGENTA, TFT_RED
};

class CyberTetris {
private:
    TFT_eSPI *tft;
    InputController *input;

    uint8_t board[TETRIS_ROWS][TETRIS_COLS];
    uint16_t boardColors[TETRIS_ROWS][TETRIS_COLS];

    int currentPiece;
    uint8_t pieceMatrix[4][4];
    int pieceX, pieceY;

    unsigned long lastDrop;
    int dropSpeed;
    int score;
    int linesCleared;
    bool gameOver;

    void spawnPiece() {
        currentPiece = random(0, 7);
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                pieceMatrix[r][c] = SHAPES[currentPiece][r][c];
            }
        }
        pieceX = 3;
        pieceY = 0;

        // Check if spawn position collides (Game Over)
        if (checkCollision(pieceX, pieceY, pieceMatrix)) {
            gameOver = true;
            SoundEngine::playExplosion();
        }
    }

    bool checkCollision(int px, int py, uint8_t matrix[4][4]) {
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                if (matrix[r][c]) {
                    int bx = px + c;
                    int by = py + r;
                    if (bx < 0 || bx >= TETRIS_COLS || by >= TETRIS_ROWS) return true;
                    if (by >= 0 && board[by][bx]) return true;
                }
            }
        }
        return false;
    }

    void rotatePiece() {
        uint8_t temp[4][4];
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                temp[c][3 - r] = pieceMatrix[r][c];
            }
        }
        if (!checkCollision(pieceX, pieceY, temp)) {
            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 4; c++) {
                    pieceMatrix[r][c] = temp[r][c];
                }
            }
            SoundEngine::playTone(750, 15);
        }
    }

    void lockPiece() {
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                if (pieceMatrix[r][c]) {
                    int bx = pieceX + c;
                    int by = pieceY + r;
                    if (by >= 0 && by < TETRIS_ROWS && bx >= 0 && bx < TETRIS_COLS) {
                        board[by][bx] = 1;
                        boardColors[by][bx] = SHAPE_COLORS[currentPiece];
                    }
                }
            }
        }

        clearLines();
        spawnPiece();
    }

    void clearLines() {
        int cleared = 0;
        for (int r = TETRIS_ROWS - 1; r >= 0; r--) {
            bool full = true;
            for (int c = 0; c < TETRIS_COLS; c++) {
                if (!board[r][c]) { full = false; break; }
            }
            if (full) {
                cleared++;
                for (int shiftR = r; shiftR > 0; shiftR--) {
                    for (int c = 0; c < TETRIS_COLS; c++) {
                        board[shiftR][c] = board[shiftR - 1][c];
                        boardColors[shiftR][c] = boardColors[shiftR - 1][c];
                    }
                }
                for (int c = 0; c < TETRIS_COLS; c++) board[0][c] = 0;
                r++; // Recheck same row
            }
        }

        if (cleared > 0) {
            linesCleared += cleared;
            score += (cleared * cleared) * 100;
            SoundEngine::playBadgeFound();
            drawHUD();
        }
    }

public:
    void init(TFT_eSPI *display, InputController *in) {
        tft = display;
        input = in;
        reset();
    }

    void reset() {
        for (int r = 0; r < TETRIS_ROWS; r++) {
            for (int c = 0; c < TETRIS_COLS; c++) {
                board[r][c] = 0;
                boardColors[r][c] = TFT_BLACK;
            }
        }
        score = 0;
        linesCleared = 0;
        gameOver = false;
        dropSpeed = 450;
        lastDrop = millis();

        spawnPiece();
        tft->fillScreen(TFT_BLACK);
        drawHUD();
    }

    void drawHUD() {
        tft->fillRect(0, 0, SCREEN_W, 26, TFT_NAVY);
        tft->setTextColor(TFT_CYAN, TFT_NAVY);
        tft->drawString("CYBER BLOCKS", 10, 6, 2);
        tft->setTextColor(TFT_YELLOW, TFT_NAVY);
        tft->drawString("SCORE: " + String(score), 150, 6, 2);
        tft->drawFastHLine(0, 26, SCREEN_W, TFT_DARKGREY);
    }

    void update() {
        if (gameOver) {
            if (input->justPressed(PIN_BTN_A)) reset();
            return;
        }

        // Move Left / Right
        if (input->justPressed(PIN_BTN_LEFT)) {
            if (!checkCollision(pieceX - 1, pieceY, pieceMatrix)) {
                pieceX--;
                SoundEngine::playTone(500, 10);
            }
        }
        if (input->justPressed(PIN_BTN_RIGHT)) {
            if (!checkCollision(pieceX + 1, pieceY, pieceMatrix)) {
                pieceX++;
                SoundEngine::playTone(500, 10);
            }
        }

        // Rotate on Button A
        if (input->justPressed(PIN_BTN_A)) {
            rotatePiece();
        }

        // Soft Drop on D-Pad DOWN
        int currentDropSpeed = input->isDown(PIN_BTN_DOWN) ? 60 : dropSpeed;

        if (millis() - lastDrop > currentDropSpeed) {
            lastDrop = millis();
            if (!checkCollision(pieceX, pieceY + 1, pieceMatrix)) {
                pieceY++;
            } else {
                lockPiece();
            }
        }
    }

    void render() {
        tft->fillRect(0, 27, SCREEN_W, SCREEN_H - 27, TFT_BLACK);

        if (gameOver) {
            tft->setTextColor(TFT_RED, TFT_BLACK);
            tft->drawCentreString("GAME OVER", SCREEN_W / 2, 120, 4);
            tft->setTextColor(TFT_YELLOW, TFT_BLACK);
            tft->drawCentreString("SCORE: " + String(score), SCREEN_W / 2, 160, 2);
            tft->setTextColor(TFT_WHITE, TFT_BLACK);
            tft->drawCentreString("PRESS A TO RETRY", SCREEN_W / 2, 190, 2);
            return;
        }

        // Draw Matrix Well Border
        tft->drawRect(BOARD_OFFSET_X - 2, BOARD_OFFSET_Y - 2, (TETRIS_COLS * BLOCK_SIZE) + 4, (TETRIS_ROWS * BLOCK_SIZE) + 4, TFT_DARKCYAN);

        // Draw Board Fixed Blocks
        for (int r = 0; r < TETRIS_ROWS; r++) {
            for (int c = 0; c < TETRIS_COLS; c++) {
                int bx = BOARD_OFFSET_X + c * BLOCK_SIZE;
                int by = BOARD_OFFSET_Y + r * BLOCK_SIZE;
                if (board[r][c]) {
                    tft->fillRoundRect(bx, by, BLOCK_SIZE - 1, BLOCK_SIZE - 1, 2, boardColors[r][c]);
                } else {
                    tft->drawPixel(bx + BLOCK_SIZE / 2, by + BLOCK_SIZE / 2, TFT_DARKGREY);
                }
            }
        }

        // Draw Current Falling Piece
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                if (pieceMatrix[r][c]) {
                    int bx = BOARD_OFFSET_X + (pieceX + c) * BLOCK_SIZE;
                    int by = BOARD_OFFSET_Y + (pieceY + r) * BLOCK_SIZE;
                    if (by >= BOARD_OFFSET_Y) {
                        tft->fillRoundRect(bx, by, BLOCK_SIZE - 1, BLOCK_SIZE - 1, 2, SHAPE_COLORS[currentPiece]);
                        tft->drawRoundRect(bx, by, BLOCK_SIZE - 1, BLOCK_SIZE - 1, 2, TFT_WHITE);
                    }
                }
            }
        }
    }
};

#endif // TETRIS_H
