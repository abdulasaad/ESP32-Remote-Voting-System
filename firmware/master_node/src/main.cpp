#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// --- Configuration ---
// Structure to receive data
// Must match the sender structure
typedef struct struct_message {
  uint8_t device_id[6];
  int vote_value;
  unsigned long timestamp;
} struct_message;

struct_message myData;
bool sessionActive = false; // Default to inactive until PC sends START

// Queue handle
#define MAX_QUEUE_SIZE 10
struct_message messageQueue[MAX_QUEUE_SIZE];
volatile int queueHead = 0;
volatile int queueTail = 0;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  if (!sessionActive) return; 

  int nextHead = (queueHead + 1) % MAX_QUEUE_SIZE;
  if (nextHead != queueTail) {
    // Copy data to queue
    memcpy(&messageQueue[queueHead], incomingData, sizeof(myData));
    queueHead = nextHead;
  } else {
    // Queue full, drop packet or handle error
  }
}

void setup() {
  // Init Serial
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("{\"event\":\"error\",\"msg\":\"Error initializing ESP-NOW\"}");
    return;
  }
  
  // Register for Receive Callback
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  Serial.println("{\"event\":\"status\",\"msg\":\"Master Ready\"}");
}

void loop() {
  // Process Queue
  if (queueHead != queueTail) {
    struct_message currentMsg = messageQueue[queueTail];
    queueTail = (queueTail + 1) % MAX_QUEUE_SIZE;
    
    // Create JSON output
    Serial.print("{\"event\":\"vote\",\"mac\":\"");
    for (int i = 0; i < 6; i++) {
        if (currentMsg.device_id[i] < 0x10) Serial.print("0");
        Serial.print(currentMsg.device_id[i], HEX);
        if (i < 5) Serial.print(":");
    }
    Serial.print("\",\"value\":");
    Serial.print(currentMsg.vote_value);
    Serial.print(",\"timestamp\":");
    Serial.print(currentMsg.timestamp);
    Serial.println("}");
  }

  // Check for Serial commands from PC
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "START_SESSION") {
      sessionActive = true;
      Serial.println("{\"event\":\"status\",\"msg\":\"Session Started\"}");
    } else if (command == "STOP_SESSION") {
      sessionActive = false;
      Serial.println("{\"event\":\"status\",\"msg\":\"Session Stopped\"}");
    } else if (command == "PING") {
      Serial.println("{\"event\":\"pong\"}");
    }
  }
}
