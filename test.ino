#include <esp_now.h>
#include <WiFi.h>

// Define relay pins
#define RELAY_1_PIN 3
#define RELAY_2_PIN 10
#define RELAY_3_PIN 19
#define RELAY_4_PIN 5

// Define switch pins
#define SWITCH_1_PIN 2
#define SWITCH_2_PIN 6
#define SWITCH_3_PIN 4
#define SWITCH_4_PIN 18

// Define debounce delay (in milliseconds)
#define DEBOUNCE_DELAY 50

// Relay states
bool relayState[4] = {LOW, LOW, LOW, LOW};

// Last switch states
bool lastSwitchState[4] = {HIGH, HIGH, HIGH, HIGH};
unsigned long lastDebounceTime[4] = {0, 0, 0, 0};

// Structure to receive ESP-NOW data
typedef struct {
  uint8_t relayNumber; // Relay number (1-4)
  uint8_t state;       // 0 = OFF, 1 = ON
} RelayCommand;

// ESP-NOW callback
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (len == sizeof(RelayCommand)) {
    RelayCommand command;
    memcpy(&command, incomingData, sizeof(command));

    if (command.relayNumber >= 1 && command.relayNumber <= 4) {
      relayState[command.relayNumber - 1] = command.state;
      digitalWrite(RELAY_1_PIN + (command.relayNumber - 1), command.state);
      Serial.printf("ESP-NOW: Relay %d set to %s\n", command.relayNumber, command.state ? "ON" : "OFF");
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize relay pins
  for (int i = 0; i < 4; i++) {
    pinMode(RELAY_1_PIN + i, OUTPUT);
    digitalWrite(RELAY_1_PIN + i, relayState[i]);
    pinMode(SWITCH_1_PIN + i, INPUT_PULLUP);
  }
  
  // Initialize WiFi and ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("ESP-NOW Ready!");
}

void loop() {
  for (int i = 0; i < 4; i++) {
    bool currentSwitchState = digitalRead(SWITCH_1_PIN + i);
    if (currentSwitchState != lastSwitchState[i]) {
      lastDebounceTime[i] = millis();
    }
    
    if ((millis() - lastDebounceTime[i]) > DEBOUNCE_DELAY) {
      if (currentSwitchState != lastSwitchState[i]) {
        lastSwitchState[i] = currentSwitchState;
        if (currentSwitchState == LOW) { // Button pressed
          relayState[i] = !relayState[i]; // Toggle relay state
          digitalWrite(RELAY_1_PIN + i, relayState[i]);
          Serial.printf("Switch %d toggled Relay %d to %s\n", i + 1, i + 1, relayState[i] ? "ON" : "OFF");
        }
      }
    }
  }
}
