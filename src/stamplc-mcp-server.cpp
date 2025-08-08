/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <Arduino.h>
#include <M5StamPLC.h>
#include <WiFi.h>
#include "dashboard_ui.h"
#include "mcp_server.h"
#include "wifi_config.h"
#include <time.h>         // For NTP time synchronization

DashboardUI dashboard_ui;
MCPServer mcp_server;

// Status light states
enum StatusLightState {
    STATUS_WIFI_DISCONNECTED,   // Red
    STATUS_TIME_SYNC_SUCCESS,   // Green
    STATUS_NORMAL_OPERATION,    // White
    STATUS_INCOMING_COMMAND     // Blue
};

StatusLightState currentStatusLight = STATUS_WIFI_DISCONNECTED;
uint32_t lastCommandTime = 0;  // Track when the last command was received

void updateStatusLight() {
    switch (currentStatusLight) {
        case STATUS_WIFI_DISCONNECTED:
            // Red
            M5StamPLC.setStatusLight(1, 0, 0);
            break;
        case STATUS_TIME_SYNC_SUCCESS:
            // Green
            M5StamPLC.setStatusLight(0, 1, 0);
            break;
        case STATUS_NORMAL_OPERATION:
            // White
            M5StamPLC.setStatusLight(1, 1, 1);
            break;
        case STATUS_INCOMING_COMMAND:
            // Blue
            M5StamPLC.setStatusLight(0, 0, 1);
            break;
    }
}

void update_time_and_date()
{
    static uint32_t time_count = 0;
    char string_buffer[100];

    if (millis() - time_count > 1000) {
        struct tm time;

        /* Get time */
        M5StamPLC.getRtcTime(&time);
        snprintf(string_buffer, sizeof(string_buffer), "%02d:%02d:%02d", time.tm_hour, time.tm_min, time.tm_sec);
        dashboard_ui.statusTime = string_buffer;

        /* Get date */
        snprintf(string_buffer, sizeof(string_buffer), "%04d.%02d.%02d", time.tm_year + 1900, time.tm_mon + 1,
                 time.tm_mday);
        dashboard_ui.statusDate = string_buffer;

        time_count = millis();
    }
}

void update_button_events()
{
    /* If button clicked, log out */
    if (M5StamPLC.BtnA.wasClicked()) {
        dashboard_ui.console_log("BtnA click");
    }
    if (M5StamPLC.BtnB.wasClicked()) {
        dashboard_ui.console_log("BtnB click");
    }
    if (M5StamPLC.BtnC.wasClicked()) {
        dashboard_ui.console_log("BtnC click");
    }

    /* Play button press and release tone */
    if (M5StamPLC.BtnA.wasPressed() || M5StamPLC.BtnB.wasPressed() || M5StamPLC.BtnC.wasPressed()) {
        M5StamPLC.tone(600, 20);
    } else if (M5StamPLC.BtnA.wasReleased() || M5StamPLC.BtnB.wasReleased() || M5StamPLC.BtnC.wasReleased()) {
        M5StamPLC.tone(800, 20);
    }
}

void update_plc_io_state()
{
    static uint32_t time_count = 0;

    if (millis() - time_count > 50) {
        for (int i = 0; i < 8; i++) {
            dashboard_ui.inputStateList[i] = M5StamPLC.readPlcInput(i);
        }
        for (int i = 0; i < 4; i++) {
            dashboard_ui.relayStateList[i] = M5StamPLC.readPlcRelay(i);
        }
        time_count = millis();
    }
}

void task_write_relay_regularly(void* param)
{
    delay(2000);

    bool relay_state = false;
    while (1) {
        relay_state = !relay_state;
        for (int i = 0; i < 4; i++) {
            M5StamPLC.writePlcRelay(i, relay_state);
            delay(500);
        }
        delay(1000);
    }
    vTaskDelete(NULL);
}

void setup()
{
    /* Init M5StamPLC */
    M5StamPLC.begin();

    /* Init dashboard UI */
    dashboard_ui.init(&M5StamPLC.Display);

    /* Set initial status light to red (not connected to WiFi) */
    currentStatusLight = STATUS_WIFI_DISCONNECTED;
    updateStatusLight();

    /* Connect to WiFi */
    dashboard_ui.console_log("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 20) {
        delay(500);
        dashboard_ui.console_log(".", false);
        attempt++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        dashboard_ui.console_log("WiFi connected!");
        std::string ipStr = WiFi.localIP().toString().c_str();
        dashboard_ui.console_log("IP: " + ipStr);
        
        /* Configure NTP and update RTC */
        dashboard_ui.console_log("Configuring NTP...");
        configTime(12600, 0, "pool.ntp.org", "time.nist.gov"); // Tehran timezone is UTC+3:30 (12600 seconds)
        
        // Wait for NTP synchronization
        dashboard_ui.console_log("Synchronizing time...");
        struct tm timeinfo;
        int attempt = 0;
        bool timeSet = false;
        
        while (!timeSet && attempt < 10) {
            delay(500);
            if (getLocalTime(&timeinfo)) {
                timeSet = true;
            }
            attempt++;
        }
        
        if (timeSet) {
            // Update RTC with NTP time
            M5StamPLC.setRtcTime(&timeinfo);
            char timeStr[64];
            strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);
            dashboard_ui.console_log("RTC updated: " + std::string(timeStr));
            
            /* Set status light to green for time sync success */
            currentStatusLight = STATUS_TIME_SYNC_SUCCESS;
            updateStatusLight();
            
            /* After brief pause, transition to normal operation (white) */
            delay(3000);
            currentStatusLight = STATUS_NORMAL_OPERATION;
            updateStatusLight();
        } else {
            dashboard_ui.console_log("NTP sync failed!");
            /* Keep the status light blue if time sync failed */
        }
        
        /* Initialize MCP server */
        dashboard_ui.console_log("Initializing MCP server...");
        mcp_server.init(&M5StamPLC, &dashboard_ui, MCP_SERVER_PORT);
        
        /* Set the command received callback */
        mcp_server.setCommandReceivedCallback([]() {
            currentStatusLight = STATUS_INCOMING_COMMAND;
            updateStatusLight();
            lastCommandTime = millis();
        });
    } else {
        dashboard_ui.console_log("WiFi connection failed!");
    }

    /* Create a task to write relay regularly */
    xTaskCreate(task_write_relay_regularly, "relay", 2048, NULL, 15, NULL);
}

void loop()
{
    M5StamPLC.update();

    update_time_and_date();
    update_button_events();
    update_plc_io_state();
    
    /* Check if we need to revert from command status to normal */
    if (currentStatusLight == STATUS_INCOMING_COMMAND && (millis() - lastCommandTime > 500)) {
        currentStatusLight = STATUS_NORMAL_OPERATION;
        updateStatusLight();
    }
    
    /* Check WiFi status and update status light if needed */
    if (WiFi.status() != WL_CONNECTED && currentStatusLight != STATUS_WIFI_DISCONNECTED) {
        currentStatusLight = STATUS_WIFI_DISCONNECTED;
        updateStatusLight();
        dashboard_ui.console_log("WiFi disconnected");
    } else if (WiFi.status() == WL_CONNECTED) {
        /* Update MCP server */
        mcp_server.update();
        
        /* If WiFi reconnected but status light shows disconnected, reset to normal operation */
        if (currentStatusLight == STATUS_WIFI_DISCONNECTED) {
            currentStatusLight = STATUS_NORMAL_OPERATION;
            updateStatusLight();
            dashboard_ui.console_log("WiFi reconnected");
        }
    }

    /* Render dashboard UI */
    dashboard_ui.render();
}
