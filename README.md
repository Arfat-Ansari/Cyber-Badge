# Shenzhen Explorer // Cyber-Badge

![Shenzhen Explorer Badge 3D Assembly](assets/cyber_badge_enclosure_3d.png)

Hey! I'm Arfat Ansari, and this is my **Shenzhen Explorer Cyber-Badge**—a handheld cyberpunk pocket console, wireless maker-mesh communicator, and 2.4GHz RF scanner that I designed from scratch for **Maker Faire Shenzhen 2026** and **Hack Club Fallen**.

---

## 💡 Why I Built This

I've always been obsessed with electronics and hardware hacking. Going to Maker Faire Shenzhen has been a huge dream of mine because Huaqiangbei is basically the hardware capital of the world. 

Most conference badges I've seen are kind of boring—they just blink a couple of LEDs, you wear them for an hour, and then they end up in a drawer forever. I wanted to build something I would genuinely want to carry around and play with every day:
1. **A pocket arcade machine:** Real tactile push buttons, a sharp 1.69" IPS color screen, and 6 classic games to play with friends.
2. **An RF scanner and radar:** A live 2.4GHz packet scanner that visualizes Wi-Fi beacon traffic as a sci-fi radar sweep while walking around Maker Faire and Huaqiangbei markets.
3. **Wireless badge-to-badge social handshake:** When two makers wearing the badge get close, the badges automatically swap digital cards and pixel avatars over ESP-NOW without needing any phone, Wi-Fi router, or internet connection.

---

## 🕹️ What It Does & How It Works

The badge runs on an **ESP32-S3** microcontroller with a 1.69" 240x280 IPS color screen, a 4-way D-pad, A/B action buttons, a piezo buzzer for 8-bit chiptune sound effects, and rechargeable USB-C lithium battery power.

```
       [ 1.69" IPS LCD ]
           +-------+
   (UP)    | 240x  |    (A) Action / Jump
(L) + (R)  |  280  |  (B) Back / Menu
  (DOWN)   +-------+
```

### Core Features:
- **Maker Social Handshake (ESP-NOW):** Automatically discovers nearby badges, exchanges maker handles, and saves them to internal flash memory.
- **Huaqiangbei RF Environment Radar:** Visualizes 2.4GHz channel activity and Wi-Fi networks in real-time as an animated radar sweep.
- **Built-in 6-in-1 Retro Cyber-Arcade:**
  - **Shenzhen Invaders:** Space Invaders arcade clone with marching alien waves, laser cannons, and bunkers.
  - **Cyber Snake:** Smooth 4-way D-pad snake game with speed boost on Button A.
  - **Shenzhen Breakout:** High-speed paddle brick breaker with destructible chip blocks.
  - **Flappy Cyber-Drone:** Precision gravity tapping through neon laser gate obstacles.
  - **Highway Racer:** Top-down highway dodging traffic cars and road stripes.
  - **Cyber Blocks:** Classic falling tetromino game with rotation and line-clear combo scoring.

---

## 🛠️ How I Built the Hardware

### 1. 3D Mechanical Enclosure (Blender 4.2)
I modeled a custom two-piece snap-fit shell in Blender, specifically dimensioned for FDM 3D printing in Tough PLA / PETG:

![3D Enclosure Upper and Lower Shells](assets/cyber_badge_enclosure_3d.png)

- **Upper Shell:** Cutouts for the 1.69" IPS screen bezel, tactile cross-pattern for the D-pad and action buttons, and internal PCB mounting bosses.
- **Lower Shell:** Pockets for the 500mAh LiPo battery, USB-C charging side slot, and component clearances.
- Blender source & 3D print STLs:
  - Master Blender File: [`cad/cyber_badge_case.blend`](cad/cyber_badge_case.blend)
  - Front Shell STL: [`cad/cyber_badge_upper_shell_print.stl`](cad/cyber_badge_upper_shell_print.stl)
  - Back Shell STL: [`cad/cyber_badge_lower_shell_print.stl`](cad/cyber_badge_lower_shell_print.stl)

### 2. Custom 2-Layer PCB
I designed the board with clean ground pours on both sides, short high-speed SPI trace routing for the ST7789 display, and an RF keepout zone under the ESP32-S3 PCB antenna for maximum wireless range.

![3D PCB Layout](assets/cyber_badge_pcb_3d.png)

- Production-ready JLCPCB Gerbers: [`Gerber_PCB1_2026-11-02.zip`](Gerber_PCB1_2026-11-02.zip)

### 3. Circuit Schematic
The circuit handles battery charging, voltage regulation, and USB-C power:

![Circuit Schematic](assets/cyber_badge_schematic.png)

- **Power Input:** USB-C (16-pin) with 5.1k CC resistors so standard USB-PD chargers work properly.
- **Battery Charging:** TP4056 lithium battery charger IC with red/blue charge status LEDs.
- **Voltage Regulator:** ME6211 3.3V 500mA ultra-low-dropout regulator.
- **Audio:** Direct PWM-driven piezo buzzer for retro 8-bit sound effects.
- Pinout reference: [`hardware/pinout.md`](hardware/pinout.md)

---

## 📊 Bill of Materials & Budget

I picked every component to keep the badge reliable, cheap, and easy to solder:

* Itemized BOM spreadsheet: [`bom.csv`](bom.csv)
* **Total Project Cost:** **$70.00 USD** (includes 10x custom PCBs, all electronic components, LiPo battery, fasteners, international tracked shipping to India, and a complete beginner soldering iron kit).

---

## 💻 Firmware & Code Structure

The firmware is written in C++ for the ESP32-S3 using PlatformIO / Arduino:
- [`firmware/main.cpp`](firmware/main.cpp) — Boot screen, state machine, menus, and game switcher.
- [`firmware/include/pins.h`](firmware/include/pins.h) — Hardware pin mappings and SPI display bus config.
- [`firmware/include/input.h`](firmware/include/input.h) — 6-button debounced input reader.
- [`firmware/include/radar.h`](firmware/include/radar.h) — 2.4GHz Wi-Fi packet scanner & radar visualizer.
- [`firmware/include/audio.h`](firmware/include/audio.h) — 8-bit chiptune sound generator using `tone()`.
- [`firmware/include/telemetry.h`](firmware/include/telemetry.h) — ESP-NOW wireless status broadcasting.
- [`firmware/include/`](firmware/include/) — Individual retro game engines (Invaders, Snake, Breakout, Flappy, Racer, Tetris).

---

## 📁 Repository Structure

```text
├── README.md                           # Project documentation and build guide
├── bom.csv                             # Full bill of materials with vendor links ($70.00)
├── Gerber_PCB1_2026-11-02.zip          # Production-ready PCB Gerbers
├── assets/                             # Real hardware and CAD captures
│   ├── cyber_badge_enclosure_3d.png    # 3D enclosure assembly (both shells)
│   ├── cyber_badge_pcb_3d.png          # 3D PCB layout render
│   ├── cyber_badge_schematic.png       # Complete circuit schematic
│   ├── enclosure_front.png             # Front shell close-up
│   └── enclosure_back.png              # Back shell close-up
├── cad/                                # 3D models and printable files
│   ├── cyber_badge_case.blend          # Master Blender 3D model
│   ├── cyber_badge_upper_shell_print.stl
│   └── cyber_badge_lower_shell_print.stl
├── firmware/                           # Complete C++ firmware
│   ├── main.cpp                        # Main entry point and state machine
│   └── include/                        # Game engines, RF radar, input, audio
└── hardware/
    └── pinout.md                       # Complete GPIO pinout table
```

---

## 👤 Author
**Arfat Ansari**  
Built for **Hack Club Fallen** and **Maker Faire Shenzhen 2026**.
