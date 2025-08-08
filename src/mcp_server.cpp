/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "mcp_server.h"
#include "dashboard_ui.h"
#include <WiFi.h>

MCPServer::MCPServer() {}

MCPServer::~MCPServer() {
    if (_server) {
        _server->end();
        delete _server;
    }
    
    for (auto& es : _event_sources) {
        delete es;
    }
    _event_sources.clear();
}

void MCPServer::init(m5::M5_STAMPLC* stamplc, DashboardUI* ui, int port) {
    _stamplc = stamplc;
    _dashboard_ui = ui;
    
    // Initialize the web server
    _server = new AsyncWebServer(port);
    
    // Log server initialization
    _dashboard_ui->console_log("Initializing MCP server...");
    
    // Register all capabilities
    registerCapabilities();
    
    // Setup HTTP and SSE endpoints
    setupHttpEndpoints();
    setupSSEEndpoints();
    
    // Start the server
    _server->begin();
    
    // Log server start
    _dashboard_ui->console_log("MCP server started on port " + std::to_string(port));
    
    // Log the IP address
    std::string ipStr = WiFi.localIP().toString().c_str();
    _dashboard_ui->console_log("IP: " + ipStr);
}

void MCPServer::update() {
    // Broadcast state to all connected clients periodically
    static uint32_t lastBroadcastTime = 0;
    if (millis() - lastBroadcastTime > 1000) { // 1 second interval
        broadcastState();
        lastBroadcastTime = millis();
    }
}

void MCPServer::registerCapabilities() {
    // Register all capabilities
    _capabilities.clear();
    
    // Read Input capability
    _capabilities.push_back({
        "readInput",
        "Read the state of a digital input",
        {"inputNumber"},
        std::bind(&MCPServer::handleReadInput, this, std::placeholders::_1, std::placeholders::_2)
    });
    
    // Write Relay capability
    _capabilities.push_back({
        "writeRelay",
        "Set the state of a relay",
        {"relayNumber", "state"},
        std::bind(&MCPServer::handleWriteRelay, this, std::placeholders::_1, std::placeholders::_2)
    });
    
    // Read Relay capability
    _capabilities.push_back({
        "readRelay",
        "Read the state of a relay",
        {"relayNumber"},
        std::bind(&MCPServer::handleReadRelay, this, std::placeholders::_1, std::placeholders::_2)
    });
    
    // Console Log capability
    _capabilities.push_back({
        "consoleLog",
        "Log a message to the device console",
        {"message"},
        std::bind(&MCPServer::handleConsoleLog, this, std::placeholders::_1, std::placeholders::_2)
    });
    
    // Get System Info capability
    _capabilities.push_back({
        "getSystemInfo",
        "Get information about the system",
        {},
        std::bind(&MCPServer::handleGetSystemInfo, this, std::placeholders::_1, std::placeholders::_2)
    });
    
    // Get IO State capability
    _capabilities.push_back({
        "getIOState",
        "Get the state of all inputs and relays",
        {},
        std::bind(&MCPServer::handleGetIOState, this, std::placeholders::_1, std::placeholders::_2)
    });
    
    // Tone capability
    _capabilities.push_back({
        "tone",
        "Play a tone with specified frequency and duration",
        {"frequency", "duration"},
        std::bind(&MCPServer::handleTone, this, std::placeholders::_1, std::placeholders::_2)
    });
    
    // Get Time capability
    _capabilities.push_back({
        "getTime",
        "Get the current time from the RTC",
        {},
        std::bind(&MCPServer::handleGetTime, this, std::placeholders::_1, std::placeholders::_2)
    });
    
    // Set Time capability
    _capabilities.push_back({
        "setTime",
        "Set the RTC time",
        {"year", "month", "day", "hour", "minute", "second"},
        std::bind(&MCPServer::handleSetTime, this, std::placeholders::_1, std::placeholders::_2)
    });
}

void MCPServer::setupHttpEndpoints() {
    // Root endpoint - serve a simple HTML page
    _server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        String html = "<html><head><title>M5StamPLC MCP Server</title></head>";
        html += "<body style='font-family: Arial, sans-serif; margin: 20px;'>";
        html += "<h1>M5StamPLC MCP Server</h1>";
        html += "<p>This is an MCP (Model Context Protocol) compliant server for M5StamPLC.</p>";
        html += "<h2>Available Endpoints:</h2>";
        html += "<ul>";
        html += "<li><a href='/mcp'>/mcp</a> - MCP API endpoint (POST)</li>";
        html += "<li><a href='/events'>/events</a> - SSE endpoint for real-time updates</li>";
        html += "<li><a href='/capabilities'>/capabilities</a> - List available capabilities</li>";
        html += "</ul>";
        html += "</body></html>";
        request->send(200, "text/html", html);
    });
    
    // Capabilities endpoint - list all available capabilities
    _server->on("/capabilities", HTTP_GET, [this](AsyncWebServerRequest *request) {
        DynamicJsonDocument doc(4096);
        JsonArray capabilities = doc.createNestedArray("capabilities");
        
        for (const auto& capability : _capabilities) {
            JsonObject cap = capabilities.createNestedObject();
            cap["name"] = capability.name;
            cap["description"] = capability.description;
            
            JsonArray params = cap.createNestedArray("parameters");
            for (const auto& param : capability.parameters) {
                params.add(param);
            }
        }
        
        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });
    
    // MCP endpoint - handle JSON-RPC style requests
    _server->on("/mcp", HTTP_POST, [](AsyncWebServerRequest *request) {
        // Just acknowledge the request, actual processing happens in onBody
        request->send(200);
    }, 
    [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        // Handle file uploads (not used)
    },
    [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        // Process the request body
        DynamicJsonDocument requestDoc(4096);
        DynamicJsonDocument responseDoc(4096);
        
        DeserializationError error = deserializeJson(requestDoc, data, len);
        
        if (error) {
            responseDoc["error"] = error.c_str();
            responseDoc["success"] = false;
        } else {
            // Process as JSON-RPC
            responseDoc["jsonrpc"] = "2.0";
            
            // Copy id if present
            if (requestDoc.containsKey("id")) {
                responseDoc["id"] = requestDoc["id"];
            }
            
            // Handle the request
            try {
                this->handleJsonRPC(requestDoc, responseDoc);
            } catch (const std::exception& e) {
                responseDoc["error"]["code"] = -32000;
                responseDoc["error"]["message"] = e.what();
            }
        }
        
        String response;
        serializeJson(responseDoc, response);
        request->send(200, "application/json", response);
        
        // Log the request (for debugging)
        _dashboard_ui->console_log("MCP request processed");
    });
}

void MCPServer::setupSSEEndpoints() {
    // Create an event source on /events
    AsyncEventSource* events = new AsyncEventSource("/events");
    
    // Set up client connect handler
    events->onConnect([this](AsyncEventSourceClient* client) {
        if (client->lastId()) {
            _dashboard_ui->console_log("Client reconnected");
        } else {
            _dashboard_ui->console_log("New SSE client connected");
        }
        
        // Send initial state
        DynamicJsonDocument stateDoc(1024);
        JsonObject state = stateDoc.createNestedObject("state");
        
        // Add input states
        JsonArray inputs = state.createNestedArray("inputs");
        for (int i = 0; i < 8; i++) {
            inputs.add(_stamplc->readPlcInput(i));
        }
        
        // Add relay states
        JsonArray relays = state.createNestedArray("relays");
        for (int i = 0; i < 4; i++) {
            relays.add(_stamplc->readPlcRelay(i));
        }
        
        // Add timestamp
        state["timestamp"] = millis();
        
        // Send to this client
        String stateStr;
        serializeJson(stateDoc, stateStr);
        client->send(stateStr.c_str(), "state", millis(), 1000);
    });
    
    // Add the event source to the server
    _server->addHandler(events);
    
    // Store the event source for later use
    _event_sources.push_back(events);
}

void MCPServer::broadcastState() {
    // Skip if no clients connected
    if (_event_sources.empty() || _event_sources[0]->count() == 0) {
        return;
    }
    
    // Create a state document
    DynamicJsonDocument stateDoc(1024);
    JsonObject state = stateDoc.createNestedObject("state");
    
    // Add input states
    JsonArray inputs = state.createNestedArray("inputs");
    for (int i = 0; i < 8; i++) {
        inputs.add(_stamplc->readPlcInput(i));
    }
    
    // Add relay states
    JsonArray relays = state.createNestedArray("relays");
    for (int i = 0; i < 4; i++) {
        relays.add(_stamplc->readPlcRelay(i));
    }
    
    // Add timestamp
    state["timestamp"] = millis();
    
    // Serialize and broadcast
    String stateStr;
    serializeJson(stateDoc, stateStr);
    
    // Send to all connected clients
    for (auto& es : _event_sources) {
        es->send(stateStr.c_str(), "state", millis(), 1000);
    }
}

void MCPServer::handleJsonRPC(JsonDocument& request, JsonDocument& response) {
    // Check if this is a JSON-RPC request
    if (!request.containsKey("method")) {
        response["error"]["code"] = -32600;
        response["error"]["message"] = "Invalid Request - missing method";
        return;
    }
    
    // Get the method name
    String methodName = request["method"].as<String>();
    
    // Find the capability
    bool found = false;
    for (const auto& capability : _capabilities) {
        if (capability.name == methodName.c_str()) {
            // Create temp JsonDocument objects for params and result
            DynamicJsonDocument paramsDoc(1024);
            DynamicJsonDocument resultDoc(1024);
            
            if (request.containsKey("params")) {
                paramsDoc = request["params"];
            }
            
            // Execute the handler
            capability.handler(paramsDoc, resultDoc);
            
            // Copy result back to response
            response["result"] = resultDoc;
            found = true;
            break;
        }
    }
    
    if (!found) {
        response["error"]["code"] = -32601;
        response["error"]["message"] = "Method not found: " + methodName;
    } else {
        response["success"] = true;
    }
}

// ==== Capability Handlers ====

void MCPServer::handleReadInput(JsonDocument& params, JsonDocument& result) {
    int inputNumber = params["inputNumber"];
    
    // Validate input number
    if (inputNumber < 0 || inputNumber >= 8) {
        throw std::runtime_error("Invalid input number (must be 0-7)");
    }
    
    // Read the input
    bool state = _stamplc->readPlcInput(inputNumber);
    
    // Return the result
    result["state"] = state;
}

void MCPServer::handleWriteRelay(JsonDocument& params, JsonDocument& result) {
    int relayNumber = params["relayNumber"];
    bool state = params["state"];
    
    // Validate relay number
    if (relayNumber < 0 || relayNumber >= 4) {
        throw std::runtime_error("Invalid relay number (must be 0-3)");
    }
    
    // Set the relay
    _stamplc->writePlcRelay(relayNumber, state);
    
    // Return success
    result["success"] = true;
}

void MCPServer::handleReadRelay(JsonDocument& params, JsonDocument& result) {
    int relayNumber = params["relayNumber"];
    
    // Validate relay number
    if (relayNumber < 0 || relayNumber >= 4) {
        throw std::runtime_error("Invalid relay number (must be 0-3)");
    }
    
    // Read the relay
    bool state = _stamplc->readPlcRelay(relayNumber);
    
    // Return the result
    result["state"] = state;
}

void MCPServer::handleConsoleLog(JsonDocument& params, JsonDocument& result) {
    String message = params["message"].as<String>();
    
    // Log the message
    _dashboard_ui->console_log(message.c_str());
    
    // Return success
    result["success"] = true;
}

void MCPServer::handleGetSystemInfo(JsonDocument& params, JsonDocument& result) {
    // Get system information
    result["device"] = "M5StamPLC";
    result["freeHeap"] = ESP.getFreeHeap();
    result["uptime"] = millis();
    result["ip"] = WiFi.localIP().toString().c_str();
    result["wifiSSID"] = WiFi.SSID().c_str();
    result["wifiRSSI"] = WiFi.RSSI();
}

void MCPServer::handleGetIOState(JsonDocument& params, JsonDocument& result) {
    // Create arrays for inputs and relays
    JsonArray inputs = result.createNestedArray("inputs");
    JsonArray relays = result.createNestedArray("relays");
    
    // Read all inputs
    for (int i = 0; i < 8; i++) {
        inputs.add(_stamplc->readPlcInput(i));
    }
    
    // Read all relays
    for (int i = 0; i < 4; i++) {
        relays.add(_stamplc->readPlcRelay(i));
    }
}

void MCPServer::handleTone(JsonDocument& params, JsonDocument& result) {
    int frequency = params["frequency"];
    int duration = params["duration"];
    
    // Validate parameters
    if (frequency < 0 || frequency > 20000) {
        throw std::runtime_error("Invalid frequency (must be 0-20000)");
    }
    
    if (duration < 0 || duration > 10000) {
        throw std::runtime_error("Invalid duration (must be 0-10000)");
    }
    
    // Play the tone
    _stamplc->tone(frequency, duration);
    
    // Return success
    result["success"] = true;
}

void MCPServer::handleGetTime(JsonDocument& params, JsonDocument& result) {
    // Get the current time
    struct tm time;
    _stamplc->getRtcTime(&time);
    
    // Format the result
    result["year"] = time.tm_year + 1900;
    result["month"] = time.tm_mon + 1;
    result["day"] = time.tm_mday;
    result["hour"] = time.tm_hour;
    result["minute"] = time.tm_min;
    result["second"] = time.tm_sec;
    
    // Format string representations
    char timeStr[9];
    char dateStr[11];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", time.tm_hour, time.tm_min, time.tm_sec);
    snprintf(dateStr, sizeof(dateStr), "%04d.%02d.%02d", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday);
    
    result["timeString"] = timeStr;
    result["dateString"] = dateStr;
}

void MCPServer::handleSetTime(JsonDocument& params, JsonDocument& result) {
    // Get parameters
    int year = params["year"];
    int month = params["month"];
    int day = params["day"];
    int hour = params["hour"];
    int minute = params["minute"];
    int second = params["second"];
    
    // Validate parameters
    if (year < 2000 || year > 2099) {
        throw std::runtime_error("Invalid year (must be 2000-2099)");
    }
    
    if (month < 1 || month > 12) {
        throw std::runtime_error("Invalid month (must be 1-12)");
    }
    
    if (day < 1 || day > 31) {
        throw std::runtime_error("Invalid day (must be 1-31)");
    }
    
    if (hour < 0 || hour > 23) {
        throw std::runtime_error("Invalid hour (must be 0-23)");
    }
    
    if (minute < 0 || minute > 59) {
        throw std::runtime_error("Invalid minute (must be 0-59)");
    }
    
    if (second < 0 || second > 59) {
        throw std::runtime_error("Invalid second (must be 0-59)");
    }
    
    // Create a tm structure
    struct tm time;
    time.tm_year = year - 1900;
    time.tm_mon = month - 1;
    time.tm_mday = day;
    time.tm_hour = hour;
    time.tm_min = minute;
    time.tm_sec = second;
    
    // Set the time
    _stamplc->setRtcTime(&time);
    
    // Return success
    result["success"] = true;
}
