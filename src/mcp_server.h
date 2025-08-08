/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <Arduino.h>
#include <M5StamPLC.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include <queue>
#include <functional>
#include <vector>
#include <string>

// Forward declaration
class DashboardUI;

class MCPServer {
public:
    MCPServer();
    ~MCPServer();

    // Initialize the MCP server
    void init(m5::M5_STAMPLC* stamplc, DashboardUI* ui, int port = 80);
    
    // Update the server - call this in the main loop
    void update();

    // MCP specific methods
    void registerCapabilities();
    void handleJsonRPC(JsonDocument& request, JsonDocument& response);
    
    // Set command received callback function
    typedef std::function<void()> CommandReceivedCallback;
    void setCommandReceivedCallback(CommandReceivedCallback callback) {
        _commandReceivedCallback = callback;
    }

private:
    // MCP Server capabilities
    struct Capability {
        std::string name;
        std::string description;
        std::vector<std::string> parameters;
        std::function<void(JsonDocument&, JsonDocument&)> handler;
    };

    // MCP Server components
    m5::M5_STAMPLC* _stamplc = nullptr;
    DashboardUI* _dashboard_ui = nullptr;
    AsyncWebServer* _server = nullptr;
    std::vector<AsyncEventSource*> _event_sources;
    std::vector<Capability> _capabilities;

    // MCP specific methods
    void setupHttpEndpoints();
    void setupSSEEndpoints();
    void broadcastState();
    
    // SSE event queue management
    std::queue<std::string> _event_queue;
    
    // Command received callback
    CommandReceivedCallback _commandReceivedCallback = nullptr;
    
    // Capability handlers
    void handleReadInput(JsonDocument& params, JsonDocument& result);
    void handleWriteRelay(JsonDocument& params, JsonDocument& result);
    void handleReadRelay(JsonDocument& params, JsonDocument& result);
    void handleConsoleLog(JsonDocument& params, JsonDocument& result);
    void handleGetSystemInfo(JsonDocument& params, JsonDocument& result);
    void handleGetIOState(JsonDocument& params, JsonDocument& result);
    void handleTone(JsonDocument& params, JsonDocument& result);
    void handleGetTime(JsonDocument& params, JsonDocument& result);
    void handleSetTime(JsonDocument& params, JsonDocument& result);
    void handleGetTemperature(JsonDocument& params, JsonDocument& result);
    void handleGetPowerVoltage(JsonDocument& params, JsonDocument& result);
    void handleGetIoCurrent(JsonDocument& params, JsonDocument& result);
    void handleGetSensorData(JsonDocument& params, JsonDocument& result);
};
