// ==============================================================
// 6-Button Tactile Input & Debouncing
// Author: Arfat Ansari
// Handles the 4-way D-pad (Up/Down/Left/Right) and A/B buttons
// Buttons are wired active-low with the ESP32 internal pullups
// ==============================================================

#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>
#include "pins.h"

struct ButtonState {
    uint8_t pin;
    bool currentState;
    bool previousState;
    unsigned long lastDebounceTime;
};

class InputController {
private:
    // 25ms debounce delay to ignore switch chatter
    static const unsigned long DEBOUNCE_DELAY = 25;

    ButtonState btnUp    = {PIN_BTN_UP, HIGH, HIGH, 0};
    ButtonState btnDown  = {PIN_BTN_DOWN, HIGH, HIGH, 0};
    ButtonState btnLeft  = {PIN_BTN_LEFT, HIGH, HIGH, 0};
    ButtonState btnRight = {PIN_BTN_RIGHT, HIGH, HIGH, 0};
    ButtonState btnA     = {PIN_BTN_A, HIGH, HIGH, 0};
    ButtonState btnB     = {PIN_BTN_B, HIGH, HIGH, 0};

    void updateButton(ButtonState &btn) {
        bool reading = digitalRead(btn.pin);
        btn.previousState = btn.currentState;
        if (reading != btn.previousState) {
            btn.lastDebounceTime = millis();
        }
        if ((millis() - btn.lastDebounceTime) > DEBOUNCE_DELAY) {
            btn.currentState = reading;
        }
    }

public:
    void init() {
        // Enable internal pullups so we don't need external resistors on the PCB
        pinMode(PIN_BTN_UP, INPUT_PULLUP);
        pinMode(PIN_BTN_DOWN, INPUT_PULLUP);
        pinMode(PIN_BTN_LEFT, INPUT_PULLUP);
        pinMode(PIN_BTN_RIGHT, INPUT_PULLUP);
        pinMode(PIN_BTN_A, INPUT_PULLUP);
        pinMode(PIN_BTN_B, INPUT_PULLUP);
    }

    void update() {
        updateButton(btnUp);
        updateButton(btnDown);
        updateButton(btnLeft);
        updateButton(btnRight);
        updateButton(btnA);
        updateButton(btnB);
    }

    // Returns true while the button is currently held down
    bool isDown(uint8_t pin) {
        switch(pin) {
            case PIN_BTN_UP:    return btnUp.currentState == LOW;
            case PIN_BTN_DOWN:  return btnDown.currentState == LOW;
            case PIN_BTN_LEFT:  return btnLeft.currentState == LOW;
            case PIN_BTN_RIGHT: return btnRight.currentState == LOW;
            case PIN_BTN_A:     return btnA.currentState == LOW;
            case PIN_BTN_B:     return btnB.currentState == LOW;
            default: return false;
        }
    }

    // Returns true only on the single frame the button is first tapped
    bool justPressed(uint8_t pin) {
        switch(pin) {
            case PIN_BTN_UP:    return btnUp.currentState == LOW && btnUp.previousState == HIGH;
            case PIN_BTN_DOWN:  return btnDown.currentState == LOW && btnDown.previousState == HIGH;
            case PIN_BTN_LEFT:  return btnLeft.currentState == LOW && btnLeft.previousState == HIGH;
            case PIN_BTN_RIGHT: return btnRight.currentState == LOW && btnRight.previousState == HIGH;
            case PIN_BTN_A:     return btnA.currentState == LOW && btnA.previousState == HIGH;
            case PIN_BTN_B:     return btnB.currentState == LOW && btnB.previousState == HIGH;
            default: return false;
        }
    }

    // Helper to detect if user pressed any button (used to exit boot screen)
    bool anyPressed() {
        return justPressed(PIN_BTN_UP) ||
               justPressed(PIN_BTN_DOWN) ||
               justPressed(PIN_BTN_LEFT) ||
               justPressed(PIN_BTN_RIGHT) ||
               justPressed(PIN_BTN_A) ||
               justPressed(PIN_BTN_B);
    }
};

#endif // INPUT_H
