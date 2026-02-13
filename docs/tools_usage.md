# PC Tools Guide

This project includes two Python tools to help you manage the voting system.

## Prerequisites
1.  Python 3 installed.
2.  Install dependencies:
    ```bash
    cd "tools"
    # Create valid environment if needed
    python3 -m venv venv
    source venv/bin/activate
    pip install pyserial esptool
    ```

## 1. Vote Receiver (`tools/vote_receiver.py`)
This script connects to the **Master Node** and displays votes in real-time. It acts as a reference implementation for your PC software.

**Usage:**
1.  Connect the Master Node to your PC.
2.  Run:
    ```bash
    python tools/vote_receiver.py
    ```
3.  Select the COM port if prompted.
4.  The script will send `START_SESSION` automatically.
5.  Votes will appear in the specific format: `MAC | Value | Timestamp`.
6.  Press `Ctrl+C` to stop and view the final tally.

## 2. Mass Flasher (`tools/flash_voters.py`)
This script helps you flash the firmware to your 100+ **Voting Nodes** quickly.

**Usage:**
1.  Ensure you have built the project successfully in PlatformIO (`pio run`).
    - This generates the `firmware.bin` file.
2.  Run:
    ```bash
    python tools/flash_voters.py
    ```
3.  The script will prompt you: `🔌 Connect a NEW Voting Device...`
4.  Connect a voting node via USB.
5.  Press Enter. It will detect the port and flash the firmware.
6.  Once done, disconnect and repeat for the next device.

**Note:**
- This script assumes a standard ESP32-C3 partition scheme.
- If it fails to find `boot_app0.bin`, check your PlatformIO packages path.
