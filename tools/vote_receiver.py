import serial
import serial.tools.list_ports
import json
import time
import sys

def get_serial_port():
    ports = list(serial.tools.list_ports.comports())
    if not ports:
        print("No serial ports found.")
        return None
    
    print("Available Ports:")
    for i, p in enumerate(ports):
        print(f"{i}: {p.device} - {p.description}")
    
    if len(ports) == 1:
        print(f"Auto-selecting {ports[0].device}")
        return ports[0].device
        
    try:
        selection = int(input("Select port index: "))
        return ports[selection].device
    except:
        return None

def main():
    port = get_serial_port()
    if not port:
        return

    try:
        ser = serial.Serial(port, 115200, timeout=1)
        print(f"Connected to {port}")
    except Exception as e:
        print(f"Error opening port: {e}")
        return

    # Give it a moment to reset
    time.sleep(2)

    # Start Session
    print("Sending START_SESSION...")
    ser.write(b"START_SESSION\n")

    print("Listening for votes... (Press Ctrl+C to stop)")
    print("-" * 40)
    print(f"{'MAC Address':<20} | {'Vote':<5} | {'Timestamp':<10}")
    print("-" * 40)

    votes = {}

    try:
        while True:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if not line:
                    continue
                
                try:
                    data = json.loads(line)
                    event = data.get("event")
                    
                    if event == "vote":
                        mac = data.get("mac")
                        val = data.get("value")
                        ts = data.get("timestamp")
                        
                        # Display
                        print(f"{mac:<20} | {val:<5} | {ts:<10}")
                        
                        # Store/Update tally (last vote counts)
                        votes[mac] = val
                        
                    elif event == "status":
                        print(f"[STATUS] {data.get('msg')}")
                        
                    elif event == "error":
                        print(f"[ERROR] {data.get('msg')}")
                        
                except json.JSONDecodeError:
                    # Ignore non-JSON lines (debug info etc)
                    pass
                    
    except KeyboardInterrupt:
        print("\nStopping...")
        ser.write(b"STOP_SESSION\n")
        ser.close()
        
        # Show Tally
        print("\nFinal Tally:")
        print("-" * 20)
        counts = {1:0, 2:0, 3:0, 4:0}
        for v in votes.values():
            if v in counts:
                counts[v] += 1
        
        for k, v in counts.items():
            print(f"Option {k}: {v} votes")
        print("-" * 20)

if __name__ == "__main__":
    main()
