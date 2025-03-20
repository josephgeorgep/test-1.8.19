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

// Variables to track relay states
bool relay1State = LOW;
bool relay2State = LOW;
bool relay3State = LOW;
bool relay4State = LOW;

// Track last switch state
bool lastSwitch1State = HIGH;
bool lastSwitch2State = HIGH;
bool lastSwitch3State = HIGH;
bool lastSwitch4State = HIGH;

// Track last debounce time
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long lastDebounceTime3 = 0;
unsigned long lastDebounceTime4 = 0;

// ESPNOW Data Structure
typedef struct {
  uint8_t relayNumber;
  uint8_t state;
} RelayCommand;

// ESPNOW Callback Function
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (len == sizeof(RelayCommand)) {
    RelayCommand command;
    memcpy(&command, incomingData, sizeof(command));

    switch (command.relayNumber) {
      case 1:
        relay1State = command.state;
        digitalWrite(RELAY_1_PIN, relay1State);
        break;
      case 2:
        relay2State = command.state;
        digitalWrite(RELAY_2_PIN, relay2State);
        break;
      case 3:
        relay3State = command.state;
        digitalWrite(RELAY_3_PIN, relay3State);
        break;
      case 4:
        relay4State = command.state;
        digitalWrite(RELAY_4_PIN, relay4State);
        break;
    }

    Serial.printf("Relay %d set to %s via ESPNOW\n", command.relayNumber, command.state ? "ON" : "OFF");
  }
}

// Setup Function
void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Set relay pins as OUTPUT
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  pinMode(RELAY_3_PIN, OUTPUT);
  pinMode(RELAY_4_PIN, OUTPUT);
  
  // Ensure all relays start OFF
  digitalWrite(RELAY_1_PIN, LOW);
  digitalWrite(RELAY_2_PIN, LOW);
  digitalWrite(RELAY_3_PIN, LOW);
  digitalWrite(RELAY_4_PIN, LOW);

  // Set switch pins as INPUT_PULLUP
  pinMode(SWITCH_1_PIN, INPUT_PULLUP);
  pinMode(SWITCH_2_PIN, INPUT_PULLUP);
  pinMode(SWITCH_3_PIN, INPUT_PULLUP);
  pinMode(SWITCH_4_PIN, INPUT_PULLUP);

  // Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Initialization Failed!");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("System Ready!");
}

// Function to Check Switches with Debounce
void checkSwitch(int switchPin, bool &lastSwitchState, unsigned long &lastDebounceTime, bool &relayState, int relayPin) {
  bool reading = digitalRead(switchPin);

  if (reading != lastSwitchState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading == LOW && lastSwitchState == HIGH) { // Button Pressed
      relayState = !relayState;  // Toggle relay state
      digitalWrite(relayPin, relayState);
      Serial.printf("Relay toggled via switch: %d\n", relayPin);
    }
  }
  
  lastSwitchState = reading; // Update last switch state
}

// Main Loop
void loop() {
  checkSwitch(SWITCH_1_PIN, lastSwitch1State, lastDebounceTime1, relay1State, RELAY_1_PIN);
  checkSwitch(SWITCH_2_PIN, lastSwitch2State, lastDebounceTime2, relay2State, RELAY_2_PIN);
  checkSwitch(SWITCH_3_PIN, lastSwitch3State, lastDebounceTime3, relay3State, RELAY_3_PIN);
  checkSwitch(SWITCH_4_PIN, lastSwitch4State, lastDebounceTime4, relay4State, RELAY_4_PIN);
}
