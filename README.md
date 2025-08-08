# M5StamPLC MCP Server

This is a Model Context Protocol (MCP) server implementation for the M5StamPLC device. It allows Claude AI to interact with the device through a standardized protocol.

## Features

- HTTP/SSE (Server-Sent Events) for real-time communication
- JSON-RPC style API compatible with Model Context Protocol
- Access to all M5 StamPLC features:
  - Digital input reading
  - Relay control
  - Console logging
  - System information
  - Real-time clock management
  - Buzzer control
  - Temperature sensor reading
  - Power voltage monitoring
  - IO socket current measurement
- Visual status indicators via RGB LED:
  - Red: WiFi disconnected
  - Green: Time synchronization successful
  - White: Normal operation mode
  - Blue: Incoming command received
- Automatic RTC time synchronization via NTP

## Configuration

Rename the `wifi_config-def.h` to `wifi_config.h` and Edit file to set your WiFi credentials:

```cpp
const char* WIFI_SSID = "YourWiFiSSID";     // Replace with your WiFi SSID
const char* WIFI_PASSWORD = "YourPassword"; // Replace with your WiFi password
```

## Getting Started

### Prerequisites
- PlatformIO installed (or Arduino IDE with ESP32 support)
- M5 StamPLC hardware
- WiFi network connection

### Installation

1. Clone this repository:
   ```bash
   git clone https://github.com/rezasaadat1/stamplc-mcp-server-io.git
   ```

2. Configure your WiFi credentials in `wifi_config.h`

3. Build and upload using PlatformIO:
   ```bash
   cd stamplc-mcp-server-io
   pio run --target upload
   ```

4. Once uploaded, the device will:
   - Connect to your WiFi network (LED will turn RED until connected)
   - Synchronize time via NTP (LED will turn GREEN briefly when synchronized)
   - Start the MCP server
   - Display its IP address on the console
   - Enter normal operation mode (LED will turn WHITE)

5. Use the device's IP address to interact with the MCP server via HTTP requests
   - When the device receives commands, the LED will flash BLUE briefly

### Examples

Check the `examples/` directory for sample use cases and demonstrations of the M5 StamPLC capabilities.

## API Endpoints

- `/` - HTML home page with basic information
- `/mcp` - MCP-compliant JSON-RPC API endpoint
- `/events` - SSE endpoint for real-time updates
- `/capabilities` - List of available capabilities

## MCP Capabilities

| Method | Description | Parameters |
|--------|-------------|------------|
| readInput | Read the state of a digital input | inputNumber (0-7) |
| writeRelay | Set the state of a relay | relayNumber (0-3), state (boolean) |
| readRelay | Read the state of a relay | relayNumber (0-3) |
| consoleLog | Log a message to the device console | message (string) |
| getSystemInfo | Get information about the system | none |
| getIOState | Get the state of all inputs and relays | none |
| tone | Play a tone with specified frequency and duration | frequency (0-20000), duration (0-10000) |
| getTime | Get the current time from the RTC | none |
| setTime | Set the RTC time | year, month, day, hour, minute, second |
| getTemperature | Get the current temperature reading | none |
| getPowerVoltage | Get the current power voltage reading | none |
| getIoCurrent | Get the current IO socket output current reading | none |
| getSensorData | Get all sensor data readings at once | none |

## Example API Calls

### Reading an Input

```json
{
  "jsonrpc": "2.0",
  "method": "readInput",
  "params": {
    "inputNumber": 0
  },
  "id": 1
}
```

### Setting a Relay

```json
{
  "jsonrpc": "2.0",
  "method": "writeRelay",
  "params": {
    "relayNumber": 0,
    "state": true
  },
  "id": 2
}
```

### Logging to Console

```json
{
  "jsonrpc": "2.0",
  "method": "consoleLog",
  "params": {
    "message": "Hello from Claude!"
  },
  "id": 3
}
```

### Getting Sensor Data

```json
{
  "jsonrpc": "2.0",
  "method": "getSensorData",
  "params": {},
  "id": 4
}
```

Response:
```json
{
  "jsonrpc": "2.0",
  "id": 4,
  "result": {
    "sensors": {
      "temperature": 25.5,
      "temperatureUnit": "Celsius",
      "voltage": 5.2,
      "voltageUnit": "Volts",
      "current": 0.12,
      "currentUnit": "Amps"
    }
  },
  "success": true
}
```

### Getting Temperature

```json
{
  "jsonrpc": "2.0",
  "method": "getTemperature",
  "params": {},
  "id": 5
}
```

## Using with Claude

Claude can communicate with this MCP server to monitor and control the M5StamPLC device. Here's an example prompt:

```
I have an M5StamPLC device running an MCP server at http://192.168.1.100. 
Can you help me control it via API calls? I'd like to:
1. Check the status of all inputs and outputs
2. Turn on relay 2
3. Log a message to the console
4. Get the current temperature, voltage, and current readings
```

Claude will be able to generate the appropriate API calls to interact with the device.

## Real-time Updates

Connect to the `/events` SSE endpoint to receive real-time updates about the state of the device. Events are sent as JSON objects with the following format:

```json
{
  "state": {
    "inputs": [false, false, true, false, false, false, false, false],
    "relays": [true, false, true, false],
    "sensors": {
      "temperature": 25.5,
      "voltage": 5.2,
      "current": 0.12
    },
    "timestamp": 123456789
  }
}
```

## Security Considerations

This implementation does not include authentication or encryption. For production use, consider adding:

- HTTPS support
- API authentication
- Input validation
- Rate limiting

## Recent Updates

### August 8, 2025: Added Sensor Data Capabilities
- Added new MCP capabilities to access M5StamPLC sensor data:
  - `getTemperature`: Read the current temperature in Celsius
  - `getPowerVoltage`: Read the current power voltage in Volts
  - `getIoCurrent`: Read the current IO socket output current in Amps
  - `getSensorData`: Get all sensor readings in a single call
- Enhanced real-time SSE updates to include sensor data
- Updated the `getSystemInfo` endpoint to include sensor readings
- Improved the state broadcast with sensor data objects

### August 8, 2025: Added Status LED Indicator
- Implemented visual status feedback via the onboard RGB LED:
  - Red: WiFi disconnected
  - Green: Time synchronization successful
  - White: Normal operation mode
  - Blue: Incoming command received
- Added command received callback functionality
- Improved error handling and recovery for WiFi disconnections

### August 8, 2025: Added RTC Time NTP Synchronization
- Implemented automatic time synchronization with NTP servers
- Configured timezone settings
- Added RTC update from network time
- Improved logging of time synchronization status

### August 8, 2025: Initial MCP Server Implementation
- Created basic MCP server architecture
- Implemented HTTP and SSE endpoints
- Added JSON-RPC style API compatible with Model Context Protocol
- Implemented core capabilities for input reading, relay control, and system information

These additions allow for comprehensive monitoring and control of the M5StamPLC device through the MCP interface.

## Development Roadmap

The following features are planned for future releases:

- Data logging capabilities for sensor readings
- Threshold-based alerts for temperature, voltage, and current
- Extended visualization in the web dashboard
- Historical data charts for sensor readings
- WebSocket support for more efficient real-time communication
- MQTT protocol integration for IoT applications

## Contributing

Contributions to the M5StamPLC MCP Server project are welcome! Here's how you can contribute:

1. **Bug Reports**: If you find a bug, please create an issue with a detailed description.
2. **Feature Requests**: Suggest new features or improvements.
3. **Code Contributions**: 
   - Fork the repository
   - Create a feature branch (`git checkout -b feature/amazing-feature`)
   - Commit your changes (`git commit -m 'Add some amazing feature'`)
   - Push to the branch (`git push origin feature/amazing-feature`)
   - Open a Pull Request

Please ensure your code follows the existing style and includes appropriate tests.

## License

MIT License
