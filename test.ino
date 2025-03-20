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

// Define debounce delay
#define DEBOUNCE_DELAY 50

// Variables to track the last debounce times
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long lastDebounceTime3 = 0;
unsigned long lastDebounceTime4 = 0;

// Structure to receive data
typedef struct {
  uint8_t relayNumber; // Relay number (1, 2, 3, or 4)
  uint8_t state;       // 0 = OFF, 1 = ON
} RelayCommand;

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (len == sizeof(RelayCommand)) {
    RelayCommand command;
    memcpy(&command, incomingData, sizeof(command));

    // Update relay based on received command
    switch (command.relayNumber) {
      case 1:
        digitalWrite(RELAY_1_PIN, command.state);
        break;
      case 2:
        digitalWrite(RELAY_2_PIN, command.state);
        break;
      case 3:
        digitalWrite(RELAY_3_PIN, command.state);
        break;
      case 4:
        digitalWrite(RELAY_4_PIN, command.state);
        break;
    }
    Serial.printf("Relay %d set to %s via ESPNOW\n", command.relayNumber, command.state ? "ON" : "OFF");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize relay pins
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  pinMode(RELAY_3_PIN, OUTPUT);
  pinMode(RELAY_4_PIN, OUTPUT);
  digitalWrite(RELAY_1_PIN, LOW);
  digitalWrite(RELAY_2_PIN, LOW);
  digitalWrite(RELAY_3_PIN, LOW);
  digitalWrite(RELAY_4_PIN, LOW);

  // Initialize switch pins with pull-ups
  pinMode(SWITCH_1_PIN, INPUT_PULLUP);
  pinMode(SWITCH_2_PIN, INPUT_PULLUP);
  pinMode(SWITCH_3_PIN, INPUT_PULLUP);
  pinMode(SWITCH_4_PIN, INPUT_PULLUP);

  // Initialize WiFi in STA mode
  WiFi.mode(WIFI_STA);
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Relay Controller Ready!");
}

void loop() {
  handleSwitch(SWITCH_1_PIN, RELAY_1_PIN, lastDebounceTime1);
  handleSwitch(SWITCH_2_PIN, RELAY_2_PIN, lastDebounceTime2);
  handleSwitch(SWITCH_3_PIN, RELAY_3_PIN, lastDebounceTime3);
  handleSwitch(SWITCH_4_PIN, RELAY_4_PIN, lastDebounceTime4);
}

void handleSwitch(int switchPin, int relayPin, unsigned long &lastDebounceTime) {
  static bool lastSwitchState = HIGH;  // Default HIGH due to pull-up
  bool currentSwitchState = digitalRead(switchPin);

  if (currentSwitchState != lastSwitchState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (currentSwitchState == LOW && lastSwitchState == HIGH) {
      // Toggle the relay state
      bool newState = !digitalRead(relayPin);
      digitalWrite(relayPin, newState);
      Serial.printf("Relay on pin %d toggled via switch\n", relayPin);
    }
  }
  lastSwitchState = currentSwitchState;
}
