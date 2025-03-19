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
bool relay1State = LOW;
bool relay2State = LOW;
bool relay3State = LOW;
bool relay4State = LOW;

// Last switch states
bool lastSwitch1State = HIGH;
bool lastSwitch2State = HIGH;
bool lastSwitch3State = HIGH;
bool lastSwitch4State = HIGH;

// Debounce timers
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long lastDebounceTime3 = 0;
unsigned long lastDebounceTime4 = 0;

// Structure for ESP-NOW data
typedef struct {
  uint8_t relayNumber; // Relay number (1-4)
  uint8_t state;       // 0 = OFF, 1 = ON
} RelayCommand;

// ESP-NOW receive callback
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (len == sizeof(RelayCommand)) {
    RelayCommand command;
    memcpy(&command, incomingData, sizeof(command));

    Serial.printf("ESP-NOW: Relay %d set to %s\n", command.relayNumber, command.state ? "ON" : "OFF");

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
      default:
        Serial.println("Invalid relay number received!");
        break;
    }
  } else {
    Serial.println("Invalid ESP-NOW data length!");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Relay Controller Initializing...");

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
  Serial.print("Relay Controller MAC: ");
  Serial.println(WiFi.macAddress());

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register ESP-NOW receive callback
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Relay Controller Ready!");
}

// Function to handle switch toggling
void handleSwitch(int switchPin, bool &lastSwitchState, unsigned long &lastDebounceTime, bool &relayState, int relayPin) {
  bool currentSwitchState = digitalRead(switchPin);
  
  if (currentSwitchState != lastSwitchState) { // Detect state change
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) { // Debounce period
    if (currentSwitchState == LOW && lastSwitchState == HIGH) { // Button press detected
      relayState = !relayState; // Toggle relay state
      digitalWrite(relayPin, relayState);
      Serial.printf("Manual: Relay %d toggled %s\n", relayPin, relayState ? "ON" : "OFF");
    }
  }

  lastSwitchState = currentSwitchState; // Update last state
}

void loop() {
  handleSwitch(SWITCH_1_PIN, lastSwitch1State, lastDebounceTime1, relay1State, RELAY_1_PIN);
  handleSwitch(SWITCH_2_PIN, lastSwitch2State, lastDebounceTime2, relay2State, RELAY_2_PIN);
  handleSwitch(SWITCH_3_PIN, lastSwitch3State, lastDebounceTime3, relay3State, RELAY_3_PIN);
  handleSwitch(SWITCH_4_PIN, lastSwitch4State, lastDebounceTime4, relay4State, RELAY_4_PIN);
}
