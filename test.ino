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

// Variables to track the relay states
bool relay1State = LOW;
bool relay2State = LOW;
bool relay3State = LOW;
bool relay4State = LOW;

// Variables to track the last stable switch states
bool lastSwitch1State = HIGH;
bool lastSwitch2State = HIGH;
bool lastSwitch3State = HIGH;
bool lastSwitch4State = HIGH;

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
  char logMessage[50];
  snprintf(logMessage, sizeof(logMessage), "Received data from %02X:%02X:%02X:%02X:%02X:%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println(logMessage);

  if (len == sizeof(RelayCommand)) {
    RelayCommand command;
    memcpy(&command, incomingData, sizeof(command));

    // Log the received command
    snprintf(logMessage, sizeof(logMessage), "Relay %d set to %s", command.relayNumber, command.state ? "ON" : "OFF");
    Serial.println(logMessage);

    // Update the relay state
    switch (command.relayNumber) {
      case 1:
        relay1State = command.state;
        digitalWrite(RELAY_1_PIN, relay1State);
        Serial.println("Relay 1 state updated via ESP-NOW.");
        break;
      case 2:
        relay2State = command.state;
        digitalWrite(RELAY_2_PIN, relay2State);
        Serial.println("Relay 2 state updated via ESP-NOW.");
        break;
      case 3:
        relay3State = command.state;
        digitalWrite(RELAY_3_PIN, relay3State);
        Serial.println("Relay 3 state updated via ESP-NOW.");
        break;
      case 4:
        relay4State = command.state;
        digitalWrite(RELAY_4_PIN, relay4State);
        Serial.println("Relay 4 state updated via ESP-NOW.");
        break;
      default:
        Serial.println("Invalid relay number received via ESP-NOW!");
        break;
    }
  } else {
    Serial.println("Invalid data length received via ESP-NOW!");
  }
}

void setup() {
  // Initialize Serial for logging
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

  // Initialize switch pins
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

  // Register callback for receiving data
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Relay Controller Ready!");
}

void loop() {
  // Read switch states with debouncing
  bool currentSwitch1State = digitalRead(SWITCH_1_PIN);
  if (currentSwitch1State != lastSwitch1State) {
    lastDebounceTime1 = millis();
  }
  if ((millis() - lastDebounceTime1) > DEBOUNCE_DELAY) {
    if (currentSwitch1State != lastSwitch1State) {
      lastSwitch1State = currentSwitch1State;
      if (currentSwitch1State == LOW) { // Switch 1 pressed (active low)
        relay1State = !relay1State; // Toggle Relay 1 state
        digitalWrite(RELAY_1_PIN, relay1State);
        Serial.println("Relay 1 state toggled via physical switch.");
      }
    }
  }

  bool currentSwitch2State = digitalRead(SWITCH_2_PIN);
  if (currentSwitch2State != lastSwitch2State) {
    lastDebounceTime2 = millis();
  }
  if ((millis() - lastDebounceTime2) > DEBOUNCE_DELAY) {
    if (currentSwitch2State != lastSwitch2State) {
      lastSwitch2State = currentSwitch2State;
      if (currentSwitch2State == LOW) { // Switch 2 pressed (active low)
        relay2State = !relay2State; // Toggle Relay 2 state
        digitalWrite(RELAY_2_PIN, relay2State);
        Serial.println("Relay 2 state toggled via physical switch.");
      }
    }
  }

  bool currentSwitch3State = digitalRead(SWITCH_3_PIN);
  if (currentSwitch3State != lastSwitch3State) {
    lastDebounceTime3 = millis();
  }
  if ((millis() - lastDebounceTime3) > DEBOUNCE_DELAY) {
    if (currentSwitch3State != lastSwitch3State) {
      lastSwitch3State = currentSwitch3State;
      if (currentSwitch3State == LOW) { // Switch 3 pressed (active low)
        relay3State = !relay3State; // Toggle Relay 3 state
        digitalWrite(RELAY_3_PIN, relay3State);
        Serial.println("Relay 3 state toggled via physical switch.");
      }
    }
  }

  bool currentSwitch4State = digitalRead(SWITCH_4_PIN);
  if (currentSwitch4State != lastSwitch4State) {
    lastDebounceTime4 = millis();
  }
  if ((millis() - lastDebounceTime4) > DEBOUNCE_DELAY) {
    if (currentSwitch4State != lastSwitch4State) {
      lastSwitch4State = currentSwitch4State;
      if (currentSwitch4State == LOW) { // Switch 4 pressed (active low)
        relay4State = !relay4State; // Toggle Relay 4 state
        digitalWrite(RELAY_4_PIN, relay4State);
        Serial.println("Relay 4 state toggled via physical switch.");
      }
    }
  }
}
