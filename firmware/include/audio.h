// ==============================================================
// 8-bit Buzzer Sound Effects
// Author: Arfat Ansari
// Uses simple ESP32 tone() on the piezo pin for retro chiptunes
// ==============================================================

#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>
#include "pins.h"

class SoundEngine {
public:
    static void init() {
        pinMode(PIN_BUZZER, OUTPUT);
        digitalWrite(PIN_BUZZER, LOW);
    }

    static void playTone(unsigned int freq, unsigned long durationMs) {
        tone(PIN_BUZZER, freq, durationMs);
        delay(durationMs);
        noTone(PIN_BUZZER);
    }

    // Little startup tune on boot
    static void playStartupFanfare() {
        playTone(523, 80);   // C5
        delay(30);
        playTone(659, 80);   // E5
        delay(30);
        playTone(784, 100);  // G5
        delay(40);
        playTone(1046, 200); // C6
    }

    // Pew pew sound when shooting aliens
    static void playLaser() {
        for (int freq = 1200; freq >= 350; freq -= 80) {
            tone(PIN_BUZZER, freq, 8);
            delay(5);
        }
        noTone(PIN_BUZZER);
    }

    // Simple noise burst for explosions
    static void playExplosion() {
        for (int i = 0; i < 15; i++) {
            int noise = random(100, 400);
            tone(PIN_BUZZER, noise, 12);
            delay(10);
        }
        noTone(PIN_BUZZER);
    }

    // Quick click when scrolling in menus
    static void playCursorMove() {
        playTone(880, 25);
    }
    static void playMenuMove() { playCursorMove(); }

    // Double beep when pressing A to select
    static void playSelect() {
        playTone(700, 40);
        delay(20);
        playTone(1100, 60);
    }
    static void playMenuSelect() { playSelect(); }

    // Happy chime when two badges connect over ESP-NOW
    static void playBadgeFound() {
        playTone(987, 80);   // B5
        delay(30);
        playTone(1318, 120); // E6
    }
};

#endif // AUDIO_H
