# M5StamPLC MCP Server

This is a Model Context Protocol (MCP) server implementation for the M5StamPLC device. It allows Claude AI to interact with the device through a standardized protocol.

## Features

- HTTP/SSE (Server-Sent Events) for real-time communication
- JSON-RPC style API compatible with Model Context Protocol
- Access to all M5StamPLC features:
  - Digital input reading
  - Relay control
  - Console logging
  - System information
  - Real-time clock management
  - Buzzer control

## Configuration

Edit the `wifi_config.h` file to set your WiFi credentials:

```cpp
const char* WIFI_SSID = "YourWiFiSSID";     // Replace with your WiFi SSID
const char* WIFI_PASSWORD = "YourPassword"; // Replace with your WiFi password
```

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

## Using with Claude

Claude can communicate with this MCP server to monitor and control the M5StamPLC device. Here's an example prompt:

```
I have an M5StamPLC device running an MCP server at http://192.168.1.100. 
Can you help me control it via API calls? I'd like to:
1. Check the status of all inputs and outputs
2. Turn on relay 2
3. Log a message to the console
```

Claude will be able to generate the appropriate API calls to interact with the device.

## Real-time Updates

Connect to the `/events` SSE endpoint to receive real-time updates about the state of the device. Events are sent as JSON objects with the following format:

```json
{
  "state": {
    "inputs": [false, false, true, false, false, false, false, false],
    "relays": [true, false, true, false],
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

## License

MIT License
