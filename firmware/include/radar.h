#ifndef RADAR_H
#define RADAR_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <TFT_eSPI.h>
#include "audio.h"
#include "input.h"

#define SCREEN_W 240
#define SCREEN_H 280

struct PeerBadge {
    char handle[16];
    char team[16];
    int8_t rssi;
    unsigned long lastSeen;
};

// Data payload broadcasted over ESP-NOW
typedef struct struct_message {
    char handle[16];
    char team[16];
} struct_message;

class CyberRadar {
private:
    TFT_eSPI *tft;
    InputController *input;
    int scannedNetworks;
    int currentChannel;
    unsigned long lastScanTime;
    float radarAngle;

    static const int MAX_PEERS = 5;
    PeerBadge peers[MAX_PEERS];
    int peerCount;

public:
    void init(TFT_eSPI *display, InputController *in) {
        tft = display;
        input = in;
        scannedNetworks = 0;
        currentChannel = 1;
        lastScanTime = 0;
        radarAngle = 0;
        peerCount = 0;

        WiFi.mode(WIFI_STA);
        WiFi.disconnect();

        tft->fillScreen(TFT_BLACK);
        drawHUD();
        scanNetworks();
    }

    void drawHUD() {
        tft->setTextColor(TFT_CYAN, TFT_BLACK);
        tft->drawString("2.4GHz RF PACKET RADAR", 10, 8, 2);
        tft->drawFastHLine(0, 26, SCREEN_W, TFT_DARKGREY);
        tft->setTextColor(TFT_GREEN, TFT_BLACK);
        tft->drawString("SCANNING AIRWAVES...", 10, 32, 2);
    }

    void scanNetworks() {
        scannedNetworks = WiFi.scanNetworks(true); // Asynchronous scan
        lastScanTime = millis();
    }

    void update() {
        radarAngle += 0.08;
        if (radarAngle > 6.28) radarAngle = 0;

        // Periodic Wi-Fi Channel Rescan
        if (millis() - lastScanTime > 3500) {
            int n = WiFi.scanComplete();
            if (n >= 0) {
                scannedNetworks = n;
                WiFi.scanDelete();
                WiFi.scanNetworks(true);
            }
            lastScanTime = millis();
        }
    }

    void render() {
        // Draw Radar Scope in center
        int centerX = SCREEN_W / 2;
        int centerY = 135;
        int radius = 60;

        // Draw concentric green radar rings
        tft->drawCircle(centerX, centerY, radius, TFT_DARKGREEN);
        tft->drawCircle(centerX, centerY, radius * 0.66, TFT_DARKGREEN);
        tft->drawCircle(centerX, centerY, radius * 0.33, TFT_DARKGREEN);
        tft->drawFastHLine(centerX - radius, centerY, radius * 2, TFT_DARKGREEN);
        tft->drawFastVLine(centerX, centerY - radius, radius * 2, TFT_DARKGREEN);

        // Draw sweeping radar scanner line
        int sweepX = centerX + cos(radarAngle) * radius;
        int sweepY = centerY + sin(radarAngle) * radius;
        tft->drawLine(centerX, centerY, sweepX, sweepY, TFT_GREEN);

        // Draw status readout below radar
        tft->fillRect(0, 210, SCREEN_W, 70, TFT_BLACK);
        tft->setTextColor(TFT_WHITE, TFT_BLACK);
        tft->drawString("NETWORKS FOUND: " + String(scannedNetworks), 20, 215, 2);

        tft->setTextColor(TFT_YELLOW, TFT_BLACK);
        tft->drawString("ESP-NOW BADGE BEACON: ACTIVE", 20, 235, 2);

        tft->setTextColor(TFT_CYAN, TFT_BLACK);
        tft->drawString("BROADCASTING ID: Auralious", 20, 255, 2);
    }
};

#endif // RADAR_H
