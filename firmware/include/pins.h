#ifndef PINS_H
#define PINS_H

// ==========================================
// Shenzhen Explorer Cyber-Badge Pinout
// Microcontroller: ESP32-S3-WROOM-1
// ==========================================

// Display SPI (1.69" 240x280 IPS ST7789)
#define PIN_LCD_SCL     12  // SPI Clock (SCK)
#define PIN_LCD_SDA     11  // SPI MOSI (Data)
#define PIN_LCD_RES     10  // Hardware Reset
#define PIN_LCD_DC       9  // Data / Command select
#define PIN_LCD_CS      13  // SPI Chip Select (Pin 21)

// Directional D-Pad (Active LOW with internal pull-ups)
#define PIN_BTN_UP       1  // GPIO 1
#define PIN_BTN_DOWN     2  // GPIO 2
#define PIN_BTN_LEFT     4  // GPIO 4
#define PIN_BTN_RIGHT    5  // GPIO 5

// Action Buttons
#define PIN_BTN_A        6  // Fire / Select / Jump (GPIO 6)
#define PIN_BTN_B        7  // Back / Menu / Bomb (GPIO 7)

// Audio & RGB Peripherals
#define PIN_BUZZER      14  // Piezo Buzzer PWM Tone
#define PIN_NEOPIXEL    15  // WS2812B RGB LEDs

#endif // PINS_H
