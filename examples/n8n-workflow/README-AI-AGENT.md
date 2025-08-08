# M5StamPLC MCP AI Agent for n8n

This workflow demonstrates how to use n8n's AI Agent node to interact with your M5StamPLC device via the Model Context Protocol (MCP).

## Features

- **Chat Interface**: Communicate with your M5StamPLC device using natural language
- **Real-time Updates**: Receive device state updates via Server-Sent Events (SSE)
- **Complete Device Control**: Access all MCP capabilities including:
  - Reading inputs and relays
  - Controlling relay states
  - Logging to the device console
  - Playing tones on the buzzer
  - Reading sensor data (temperature, voltage, current)
  - Getting system information

## Setup Instructions

1. **Import the Workflow**
   - In n8n, go to "Workflows" and click "Import from File"
   - Select the `Demo_ M5StamPLC MCP AI Agent.json` file

2. **Configure Model**
   - This workflow uses Ollama with the Gemma model by default
   - You can substitute with any LLM of your choice (Claude, GPT, etc.)

3. **Verify Device Connection**
   - Ensure your M5StamPLC is powered on and connected to WiFi
   - Verify the IP address in the MCP Client node (currently set to 192.168.1.236)
   - The workflow connects to:
     - `/mcp` endpoint for sending commands
     - `/events` endpoint for real-time updates

4. **Activate the Workflow**
   - Toggle the workflow to "Active" state
   - You'll receive a unique chat URL you can use to interact with the agent

## Usage Examples

Here are some example commands you can send to the AI Agent:

- "What is the current temperature reading?"
- "Turn on relay 2"
- "What's the status of all inputs and outputs?"
- "Log a message to the console saying 'Hello from n8n!'"
- "Play a 1000Hz tone for 500ms"
- "What's the power voltage right now?"
- "Is input 3 currently active?"
- "Get all sensor readings at once"

## Understanding the Workflow

1. **When chat message received**: Handles incoming messages from the chat interface
2. **AI Agent**: The core component that processes requests and decides actions
3. **Ollama Chat Model**: The language model that powers the agent's reasoning
4. **Simple Memory**: Maintains conversation context for more coherent interactions
5. **MCP Client**: Communicates with the M5StamPLC device using the MCP protocol

## Customization

- **Change the LLM**: Replace the Ollama node with any other LLM (Claude, OpenAI, etc.)
- **Adjust System Message**: Modify the instructions in the AI Agent's system message
- **Add Capabilities**: Enhance the MCP Client with additional device functions
- **Extend with Other Nodes**: Add HTTP requests, database connections, or other n8n nodes

## Troubleshooting

- **Connection Issues**: Verify your M5StamPLC's IP address and that it's on the same network
- **Missing Responses**: Check that your LLM is properly configured and responding
- **Failed Commands**: Ensure command parameters are within the valid ranges

## Further Enhancements

Consider adding:
- Webhook notifications for critical device states
- Database storage for sensor readings
- Integration with other IoT devices
- Custom dashboard for visualizing device data
