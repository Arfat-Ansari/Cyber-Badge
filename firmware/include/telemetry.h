// ==============================================================
// ESP-NOW Wireless Telemetry Broadcast
// Author: Arfat Ansari
// Broadcasts badge stats (battery, game, handle) to nearby docks
// ==============================================================

#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// Packet format shared between the badge and desktop dock
typedef struct BadgeTelemetryPacket {
    char pilot[16];
    float battery_volts;
    int battery_percent;
    char active_game[16];
    int high_score;
    bool is_docked;
} BadgeTelemetryPacket;

class TelemetryBroadcaster {
public:
    BadgeTelemetryPacket data;
    uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    unsigned long lastSendTime = 0;

    void init() {
        WiFi.mode(WIFI_STA);
        if (esp_now_init() == ESP_OK) {
            esp_now_peer_info_t peerInfo = {};
            memcpy(peerInfo.peer_addr, broadcastAddress, 6);
            peerInfo.channel = 0;
            peerInfo.encrypt = false;
            esp_now_add_peer(&peerInfo);
        }
        strcpy(data.pilot, "Arfat");
        data.battery_volts = 4.15;
        data.battery_percent = 95;
        strcpy(data.active_game, "Arcade Menu");
        data.high_score = 0;
        data.is_docked = false;
    }

    // Default periodic update
    void update() {
        if (millis() - lastSendTime > 1500) {
            lastSendTime = millis();
            esp_now_send(broadcastAddress, (uint8_t *) &data, sizeof(data));
        }
    }

    void update(const char* currentGame, int score, float volts, int pct, bool docked) {
        strncpy(data.active_game, currentGame, 15);
        data.active_game[15] = '\0';
        data.high_score = score;
        data.battery_volts = volts;
        data.battery_percent = pct;
        data.is_docked = docked;

        if (millis() - lastSendTime > 1000) {
            lastSendTime = millis();
            esp_now_send(broadcastAddress, (uint8_t *) &data, sizeof(data));
        }
    }
};

#endif // TELEMETRY_H
