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

#define DEBOUNCE_DELAY 50 // 50ms debounce time

unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long lastDebounceTime3 = 0;
unsigned long lastDebounceTime4 = 0;

bool lastSwitchState1 = HIGH;
bool lastSwitchState2 = HIGH;
bool lastSwitchState3 = HIGH;
bool lastSwitchState4 = HIGH;

// Structure to receive ESPNOW data
typedef struct {
  uint8_t relayNumber;
  uint8_t state; // 0 = OFF, 1 = ON
} RelayCommand;

// ESP-NOW Callback for receiving data
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (len == sizeof(RelayCommand)) {
    RelayCommand command;
    memcpy(&command, incomingData, sizeof(command));

    // Update relay state from ESPNOW command
    switch (command.relayNumber) {
      case 1: digitalWrite(RELAY_1_PIN, command.state); break;
      case 2: digitalWrite(RELAY_2_PIN, command.state); break;
      case 3: digitalWrite(RELAY_3_PIN, command.state); break;
      case 4: digitalWrite(RELAY_4_PIN, command.state); break;
    }
    
    Serial.printf("Relay %d set to %s via ESPNOW\n", command.relayNumber, command.state ? "ON" : "OFF");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

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

  // Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("System Ready!");
}

void loop() {
  handleSwitch(SWITCH_1_PIN, RELAY_1_PIN, lastSwitchState1, lastDebounceTime1);
  handleSwitch(SWITCH_2_PIN, RELAY_2_PIN, lastSwitchState2, lastDebounceTime2);
  handleSwitch(SWITCH_3_PIN, RELAY_3_PIN, lastSwitchState3, lastDebounceTime3);
  handleSwitch(SWITCH_4_PIN, RELAY_4_PIN, lastSwitchState4, lastDebounceTime4);
}

// Function to handle switch inputs and toggle relays independently
void handleSwitch(int switchPin, int relayPin, bool &lastSwitchState, unsigned long &lastDebounceTime) {
  bool currentSwitchState = digitalRead(switchPin);
  
  // Check if the switch state changed
  if (currentSwitchState != lastSwitchState) {
    lastDebounceTime = millis();
  }

  // Apply debounce delay
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (currentSwitchState == LOW && lastSwitchState == HIGH) { 
      // Read the actual relay state and toggle it
      bool newState = !digitalRead(relayPin);
      digitalWrite(relayPin, newState);
      Serial.printf("Relay on pin %d toggled via switch\n", relayPin);
    }
  }
  
  lastSwitchState = currentSwitchState;
}
