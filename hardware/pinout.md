# Hardware Pinout & Circuit Architecture

## Microcontroller: ESP32-S3-WROOM-1

### 1. Power Supply Circuit
* **USB-C Input (5V):** Powers the board via VBUS and charges the LiPo battery through the TP4056.
* **TP4056 LiPo Charger:**
  * Pin 1 (TEMP): Connected to GND.
  * Pin 2 (PROG): Connected via 1.2k resistor to GND (sets charge current to ~1000mA or 2k for 500mA).
  * Pin 4 (VCC): 5V from USB-C.
  * Pin 5 (BAT): Connected to LiPo Battery Positive.
  * Pin 6 & 7 (STDBY & CHRG): Red/Blue status indicator LEDs.
* **Voltage Regulator (ME6211 3.3V LDO):**
  * Takes VBAT / VBUS (via Schottky diode protection) and outputs a stable, clean 3.3V rail (up to 500mA) for the ESP32-S3 and peripherals.

### 2. Display Pinout (1.69" ST7789 SPI LCD)
| LCD Pin | ESP32-S3 GPIO | Function |
| :--- | :--- | :--- |
| **GND** | GND | Ground |
| **VCC** | 3.3V | 3.3V Power |
| **SCL** | GPIO 12 | SPI Clock (SCK) |
| **SDA** | GPIO 11 | SPI MOSI (Data) |
| **RES** | GPIO 10 | Hardware Reset |
| **DC**  | GPIO 9  | Data / Command select |
| **CS**  | GPIO 13 (Pin 21) or GPIO 8 (Pin 12) | SPI Chip Select |
| **BLK** | GPIO 7  | Backlight PWM Control |

### 3. Tactile Push Buttons (Active LOW with internal pull-up)
| Button Name | ESP32-S3 GPIO | Description |
| :--- | :--- | :--- |
| **D-PAD UP** | GPIO 1 | Up movement in menus / games |
| **D-PAD DOWN** | GPIO 2 | Down movement in menus / games |
| **D-PAD LEFT** | GPIO 4 | Left movement |
| **D-PAD RIGHT** | GPIO 5 | Right movement |
| **ACTION A** | GPIO 6 | Select / Jump / Primary Action |
| **ACTION B** | GPIO 7 | Back / Cancel / Secondary Action |

### 4. Audio & Status Indicators
| Peripheral | ESP32-S3 GPIO | Description |
| :--- | :--- | :--- |
| **Piezo Buzzer** | GPIO 14 | PWM tone output for 8-bit sound effects |
| **NeoPixels (2x)** | GPIO 15 | Addressable WS2812B RGB LEDs for badge status |
