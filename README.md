# ESP32 Remote Voting System

**A scalable, wireless voting system capable of handling 100+ voting nodes using ESP-NOW.**

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-ESP32-green.svg)

## 📖 Overview

This project provides a complete infrastructure for a remote polling/voting system. It consists of:
1.  **Voting Nodes**: Small, battery-powered ESP32 devices (ESP32-C3 Super Mini) with 4 buttons.
2.  **Master Node**: A central ESP32-S3 device that receives votes wirelessly via **ESP-NOW**.
3.  **PC Interface**: The Master Node connects via USB Serial and streams data in **JSON format** to any PC software.

The system is designed for high concurrency, low latency, and easy deployment.

## 🚀 Features

-   **Scalable**: Supports 100+ devices communicating simultaneously.
-   **Collision Avoidance**: Implements random backoff logic to ensure vote delivery.
-   **Robust Input**: State-change detection ensures only one vote per button press (no accidental double-voting).
-   **Developer Friendly**: Simple JSON Serial protocol for easy integration with Python, C#, Electron, etc.
-   **Deployment Tools**: Includes Python scripts for mass-flashing devices and monitoring votes.

## 🛠️ Hardware Requirements

-   **Master Node**: 1x ESP32-S3 (e.g., XH-S3E or DevKitC)
-   **Voting Nodes**: 100x ESP32-C3 Super Mini (or any ESP32 variant)
-   **Peripherals**: 4x Push Buttons per Voting Node (connected to GPIO 0, 1, 2, 3)

## 📂 Project Structure

```
├── firmware/
│   ├── voting_node/       # Sender Firmware (ESP32-C3)
│   └── master_node/       # Receiver Firmware (ESP32-S3)
├── tools/
│   ├── vote_receiver.py   # PC Tool: Monitor votes in real-time
│   └── flash_voters.py    # PC Tool: Automate flashing 100+ devices
├── docs/
│   ├── interface_specification.md  # Serial Protocol Documentation
│   └── tools_usage.md              # Guide for Python tools
└── platformio.ini         # Build configuration
```

## ⚡ Installation & Setup

### 1. Firmware Build
This project uses **PlatformIO**.
1.  Open the project in VS Code (with PlatformIO extension).
2.  Click the **Build** button (✓) to compile both `voting_node` and `master_node`.

### 2. Flashing the Master Node
1.  Connect the ESP32-S3 to your PC.
2.  Run the upload task for `master_node`.

### 3. Flashing Voting Nodes (Mass Deployment)
We provide a script to flash many devices quickly.
1.  Navigate to the `tools/` directory.
2.  Setup Python environment:
    ```bash
    python3 -m venv venv
    source venv/bin/activate
    pip install pyserial esptool
    ```
3.  Run the flasher:
    ```bash
    python flash_voters.py
    ```
4.  Follow the prompts: Connect a device -> Press Enter -> Repeat.

## 🖥️ Usage

### Running the System
1.  Connect the **Master Node** to your PC via USB.
2.  Power on the **Voting Nodes**.

### Monitoring Votes (Python)
Use the included script to verify the system or as a reference implementation.

```bash
python tools/vote_receiver.py
```

The script sends the `START_SESSION` command and prints votes as they arrive:

| MAC Address | Vote | Timestamp |
| :--- | :--- | :--- |
| 94:A9:90:98:15:40 | 1 | 12345 |
| A0:B1:C2:D3:E4:F5 | 3 | 12390 |

### Integrating with Custom Software
The Master Node communicates via standard Serial (115200 baud).

**Protocol:**
-   **PC -> Master**: Send `START_SESSION\n` to begin.
-   **Master -> PC**: Receives JSON Lines:
    ```json
    {"event":"vote", "mac":"94:A9:90:98:15:40", "value":1, "timestamp":12345}
    ```

See [Interface Specification](docs/interface_specification.md) for full details.

## 📄 License
This project is licensed under the MIT License.
