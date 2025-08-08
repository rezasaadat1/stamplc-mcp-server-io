# Connecting M5StamPLC to Claude via n8n

This guide explains how to replace the default Ollama model with Claude in your n8n AI Agent workflow.

## Prerequisites

- n8n installed and running
- M5StamPLC with MCP server running at 192.168.1.236
- Anthropic API key for Claude

## Setting Up Claude in n8n

1. **Add Claude Credentials**

   In n8n:
   - Go to "Settings" > "Credentials"
   - Click "Add Credential"
   - Select "Anthropic API"
   - Enter your Anthropic API key
   - Save the credential

2. **Replace Ollama with Claude**

   In your workflow:
   - Delete the "Ollama Chat Model" node
   - Add a new "Anthropic Chat Model" node
   - Configure it with:
     - Credentials: Your saved Anthropic API credentials
     - Model: claude-3-opus-20240229 (or another Claude model)
     - Temperature: 0.1-0.3 (lower for more precise responses)
   - Connect it to the AI Agent's ai_languageModel input

3. **Optimize System Message**

   Claude works best with clear instructions. Consider updating the AI Agent's system message to:

   ```
   You are Claude, an AI assistant helping users interact with an M5StamPLC device via the Model Context Protocol (MCP).

   The M5StamPLC has 8 digital inputs (0-7), 4 relay outputs (0-3), and sensors for temperature, voltage, and current. You can help monitor and control this device.

   Available MCP methods:
   - readInput: Read a digital input state (inputNumber: 0-7)
   - writeRelay: Control a relay (relayNumber: 0-3, state: true/false)
   - readRelay: Read a relay state (relayNumber: 0-3)
   - consoleLog: Log to device display (message: string)
   - getSystemInfo: Get system information
   - getIOState: Get all input/output states
   - tone: Play sound (frequency: 0-20000, duration: 0-10000)
   - getTime: Get current time
   - getTemperature: Get temperature in Celsius
   - getPowerVoltage: Get voltage in Volts
   - getIoCurrent: Get current in Amps
   - getSensorData: Get all sensor readings

   Real-time updates from the device will be available through the SSE connection.

   Be concise, precise, and helpful in your responses. When suggesting actions, be specific about the exact MCP method and parameters to use.
   ```

## Tips for Optimal Claude Performance

1. **Adjust Memory Settings**
   - Increase the window size to 10-15 for better context retention
   - Set the context size threshold to around 8000 for Claude 3 Opus

2. **Fine-tune Response Format**
   - Claude 3 can handle structured data well, so you can expect well-formatted responses

3. **Enhance MCP Client Configuration**
   - Make sure all capabilities are properly defined with accurate parameter types
   - Add detailed descriptions for each parameter to help Claude understand usage

4. **Example Claude Prompts**

   Here are some effective prompts to use with Claude:

   - "Claude, can you check the temperature sensor and let me know if it's within normal range?"
   - "I'd like to set up a pattern where relay 1 and 3 alternate on and off every few seconds. Can you help me do that?"
   - "What's the complete state of my M5StamPLC right now? Please summarize all inputs, outputs, and sensor readings."
   - "I heard a strange noise from my equipment. Can you use the device to play a diagnostic tone sequence?"

5. **Troubleshooting Claude Integration**

   If you encounter issues:
   
   - Check API key validity and usage limits
   - Verify model availability (some Claude models may have restricted access)
   - Ensure requests stay within context length limits
   - Add error handling for API rate limits

## Advanced Claude Features

Claude 3 Opus has capabilities that can enhance your M5StamPLC interaction:

1. **Complex reasoning** for diagnosing device issues
2. **Pattern recognition** for identifying anomalies in sensor data
3. **Code generation** for creating custom automation scripts
4. **Multi-step planning** for complex device control sequences

By using Claude with your M5StamPLC, you can create more sophisticated automation and monitoring solutions than with simpler models.
