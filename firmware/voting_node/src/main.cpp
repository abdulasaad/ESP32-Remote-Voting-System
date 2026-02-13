#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// --- Configuration ---
// Pins for the 4 buttons
const int BTN_PIN_1 = 0;
const int BTN_PIN_2 = 1;
const int BTN_PIN_3 = 2;
const int BTN_PIN_4 = 3;

// Broadcast Address (FF:FF:FF:FF:FF:FF) - sends to all listening ESP-NOW devices
// Or specific Master MAC if known. For simplicity/flexibility, we use Broadcast initially.
// In a 100-device setup, pairing is better, but broadcast works if only one Master is present.
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Structure to send data
// Must match the receiver structure
typedef struct struct_message {
  uint8_t device_id[6]; // MAC address of this sender
  int vote_value;       // 1, 2, 3, or 4
  unsigned long timestamp; // Time of vote to prevent duplicates
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

// Variables for button state
bool sending = false;

// LED for status
const int LED_PIN = 8; 

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if (status == ESP_NOW_SEND_SUCCESS) {
    // Flash LED fast to indicate success
    digitalWrite(LED_PIN, LOW); // On
    delay(100);
    digitalWrite(LED_PIN, HIGH);
  }
  sending = false;
}

void sendVote(int value) {
  if (sending) return; 
  sending = true;

  WiFi.macAddress(myData.device_id);
  myData.vote_value = value;
  myData.timestamp = millis();

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
    sending = false;
  }
}

// Button state tracking
bool btnPressed[4] = {false, false, false, false};
int btnPins[4] = {BTN_PIN_1, BTN_PIN_2, BTN_PIN_3, BTN_PIN_4};

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(btnPins[i], INPUT_PULLUP); // Buttons are active LOW
  }
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // LED off initially (assuming active LOW LED)

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  for (int i = 0; i < 4; i++) {
    bool currentReading = digitalRead(btnPins[i]);
    
    // Check for Press (LOW) and was not previously pressed
    if (currentReading == LOW && !btnPressed[i]) {
      // Debounce: Wait a bit and check again to confirm it's a real press
      delay(50); 
      if (digitalRead(btnPins[i]) == LOW) {
        sendVote(i + 1);
        btnPressed[i] = true; // Mark as pressed
      }
    }
    // Check for Release (HIGH) and was previously pressed
    else if (currentReading == HIGH && btnPressed[i]) {
      delay(50); // Debounce release
      if (digitalRead(btnPins[i]) == HIGH) {
        btnPressed[i] = false; // Mark as released, ready for next press
      }
    }
  }
}
