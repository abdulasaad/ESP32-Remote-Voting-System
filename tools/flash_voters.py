import os
import sys
import time
import subprocess
import serial.tools.list_ports

# Configuration
# Path to the compiled binary from PlatformIO
# This assumes you have built the project successfully at least once.
FIRMWARE_PATH = os.path.join("..", ".pio", "build", "esp32-c3-supermini", "firmware.bin")
BOOTLOADER_PATH = os.path.join("..", ".pio", "build", "esp32-c3-supermini", "bootloader.bin")
PARTITIONS_PATH = os.path.join("..", ".pio", "build", "esp32-c3-supermini", "partitions.bin")
# Boot app is standard for Arduino ESP32
BOOT_APP_PATH = os.path.expanduser("~/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin")

def get_esp32_port():
    # Helper to find a likely ESP32 port
    ports = list(serial.tools.list_ports.comports())
    candidates = []
    for p in ports:
        # Filter logic can be improved. Searching for USB Serial / JTAG usually.
        # For Super Mini (C3), it often shows as "USB Serial" or "JTAG".
        candidates.append(p.device)
    return candidates

def flash_device(port):
    print(f"Flashing device on {port}...")
    
    # Use the current python interpreter (from venv)
    python_exe = sys.executable 
    
    # Construct esptool command
    # Using typical offsets for ESP32-C3 Arduino
    cmd = [
        python_exe, "-m", "esptool",
        "--chip", "esp32c3",
        "--port", port,
        "--baud", "460800",
        "--before", "default_reset",
        "--after", "hard_reset",
        "write_flash",
        "-z",
        "--flash_mode", "dio",
        "--flash_freq", "80m",
        "--flash_size", "4MB",
        "0x0000", BOOTLOADER_PATH,
        "0x8000", PARTITIONS_PATH,
        "0xe000", BOOT_APP_PATH,
        "0x10000", FIRMWARE_PATH
    ]
    
    try:
        subprocess.run(cmd, check=True)
        print("\n✅ Flash Successful!")
        return True
    except subprocess.CalledProcessError:
        print("\n❌ Flash Failed!")
        return False

def main():
    print("=== Mass Flasher for Voting Nodes ===")
    
    if not os.path.exists(FIRMWARE_PATH):
        print(f"Error: Firmware file not found at {FIRMWARE_PATH}")
        print("Please build the voting_node project in PlatformIO first.")
        return

    # Try to find boot_app0 automatically if not hardcoded
    global BOOT_APP_PATH
    if not os.path.exists(BOOT_APP_PATH):
        # Fallback search or ask user
        # For now, let's try to assume it might be in a standard location or skip if not found (will fail)
        print(f"Warning: boot_app0.bin not found at {BOOT_APP_PATH}")
        print("You may need to adjust the path in the script.")

    processed_ports = set()

    while True:
        input("\n🔌 Connect a NEW Voting Device and press Enter (or 'q' to quit): ")
        
        current_ports = set(get_esp32_port())
        
        # Simple detection: find a port?
        # In a mass production setup, you might detect difference.
        # Here we just ask user to pick if multiple, or auto-pick if 1.
        
        if not current_ports:
            print("No devices found.")
            continue
            
        print("Found ports:", current_ports)
        target_port = list(current_ports)[0] # Default to first for speed
        
        if len(current_ports) > 1:
            target_port = input(f"Enter port to flash (default {list(current_ports)[0]}): ") or list(current_ports)[0]

        if flash_device(target_port):
            print("👉 Please disconnect the device.")
        else:
            print("⚠️ Please retry or check connections.")

if __name__ == "__main__":
    main()
