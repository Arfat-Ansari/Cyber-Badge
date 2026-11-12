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

    // 8-Bit Retro Startup Fanfare
    static void playStartupFanfare() {
        playTone(523, 80);   // C5
        delay(30);
        playTone(659, 80);   // E5
        delay(30);
        playTone(784, 100);  // G5
        delay(40);
        playTone(1046, 200); // C6
    }

    // Laser Blast (Shenzhen Invaders)
    static void playLaser() {
        for (int freq = 1200; freq >= 350; freq -= 80) {
            tone(PIN_BUZZER, freq, 8);
            delay(5);
        }
        noTone(PIN_BUZZER);
    }

    // Alien Explode
    static void playExplosion() {
        for (int i = 0; i < 15; i++) {
            int noise = random(100, 400);
            tone(PIN_BUZZER, noise, 12);
            delay(10);
        }
        noTone(PIN_BUZZER);
    }

    // Menu Navigation Bleep
    static void playMenuMove() {
        playTone(880, 25);
    }

    // Menu Select Chime
    static void playMenuSelect() {
        playTone(700, 40);
        delay(20);
        playTone(1100, 60);
    }

    // ESP-NOW Badge P2P Handshake Found
    static void playBadgeFound() {
        playTone(987, 80);   // B5
        delay(30);
        playTone(1318, 120); // E6
    }
};

#endif // AUDIO_H
