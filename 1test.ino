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

// Debounce delay
#define DEBOUNCE_DELAY 50

// Relay states
bool relayState[4] = {LOW, LOW, LOW, LOW};
unsigned long lastDebounceTime[4] = {0, 0, 0, 0};
bool lastSwitchState[4] = {HIGH, HIGH, HIGH, HIGH};

// Structure to receive data
typedef struct {
  uint8_t relayNumber;
  uint8_t state;
} RelayCommand;

// ESP-NOW callback
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (len == sizeof(RelayCommand)) {
    RelayCommand command;
    memcpy(&command, incomingData, sizeof(command));
    
    if (command.relayNumber >= 1 && command.relayNumber <= 4) {
      relayState[command.relayNumber - 1] = command.state;
      digitalWrite(RELAY_1_PIN + (command.relayNumber - 1), command.state);
      Serial.printf("Relay %d set to %s via ESP-NOW\n", command.relayNumber, command.state ? "ON" : "OFF");
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);

  for (int i = 0; i < 4; i++) {
    pinMode(RELAY_1_PIN + i, OUTPUT);
    digitalWrite(RELAY_1_PIN + i, LOW);
    pinMode(SWITCH_1_PIN + i, INPUT_PULLUP);
  }
  Serial.println("Setup Complete");
}

void loop() {
  for (int i = 0; i < 4; i++) {
    bool currentSwitchState = digitalRead(SWITCH_1_PIN + i);
    if (currentSwitchState != lastSwitchState[i]) {
      lastDebounceTime[i] = millis();
    }
    if ((millis() - lastDebounceTime[i]) > DEBOUNCE_DELAY) {
      if (currentSwitchState == LOW && lastSwitchState[i] == HIGH) {
        relayState[i] = !relayState[i];
        digitalWrite(RELAY_1_PIN + i, relayState[i]);
        Serial.printf("Switch %d toggled: Relay %d %s\n", i + 1, i + 1, relayState[i] ? "ON" : "OFF");
      }
    }
    lastSwitchState[i] = currentSwitchState;
  }
}
