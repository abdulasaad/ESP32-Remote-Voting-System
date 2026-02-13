# Master Node Interface Specification

This document describes how to interface with the Master ESP32-S3 Voting Receiver.

## Connection Details
- **Interface**: USB Serial (COM Port / `/dev/ttyUSBx`)
- **Baud Rate**: 115200
- **Data Bits**: 8, **Parity**: None, **Stop Bits**: 1

## Protocol (PC -> Master ESP)
Send these commands as strings followed by a newline (`\n`).

| Command | Description | Response |
| :--- | :--- | :--- |
| `START_SESSION` | Enables the Master to accept ESP-NOW packets. | `{"event":"status","msg":"Session Started"}` |
| `STOP_SESSION` | Disables vote acceptance. | `{"event":"status","msg":"Session Stopped"}` |
| `PING` | Checks if the device is responsive. | `{"event":"pong"}` |

## Protocol (Master ESP -> PC)
All data sent from the Master ESP to the PC is in **JSON Line** format. Each line is a valid JSON object.

### 1. Vote Received
Sent when a voting device presses a button.
```json
{
  "event": "vote",
  "mac": "AA:BB:CC:DD:EE:FF",
  "value": 1, 
  "timestamp": 12345678
}
```
- `mac`: The MAC address of the voting device. Use this to identify unique voters.
- `value`: The button pressed (1, 2, 3, or 4).
- `timestamp`: The millisecond timestamp from the voting device (can be used for deduplication if needed, though the random backoff on the sender side helps).

### 2. Status Messages
Sent in response to commands or system events.
```json
{"event":"status", "msg":"Master Ready"}
{"event":"error", "msg":"Error initializing ESP-NOW"}
```

## Integration Workflow for PC Software
1.  **Open Serial Port**: Detect the ESP32 COM port and open it at 115200 baud.
2.  **Wait for Ready**: You might receive `{"event":"status","msg":"Master Ready"}` on boot, or send `PING` to check connection.
3.  **Start Voting**: Send `START_SESSION\n`.
4.  **Listen**: Read incoming lines. Parse JSON.
    - If `event` is `vote`, log the vote for `mac`.
    - Handle duplicates in software (e.g., if the same MAC sends multiple votes, count only the last one or first one depending on rules).
5.  **Stop Voting**: Send `STOP_SESSION\n`.
