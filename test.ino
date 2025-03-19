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

// Variables to track relay states
bool relay1State = LOW;
bool relay2State = LOW;
bool relay3State = LOW;
bool relay4State = LOW;

// Variables to track last stable switch states
bool lastSwitch1State = HIGH;
bool lastSwitch2State = HIGH;
bool lastSwitch3State = HIGH;
bool lastSwitch4State = HIGH;

// Variables to track last debounce times
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long lastDebounceTime3 = 0;
unsigned long lastDebounceTime4 = 0;

// Structure to receive ESP-NOW data
typedef struct {
  uint8_t relayNumber; // Relay number (1, 2, 3, or 4)
  uint8_t state;       // 0 = OFF, 1 = ON
} RelayCommand;

// Callback when ESP-NOW data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (len == sizeof(RelayCommand)) {
    RelayCommand command;
    memcpy(&command, incomingData, sizeof(command));

    Serial.printf("ESP-NOW: Relay %d set to %s\n", command.relayNumber, command.state ? "ON" : "OFF");

    // Update the relay state based on ESP-NOW command
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
        Serial.println("Invalid relay number received via ESP-NOW!");
        break;
    }
  }
}

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  Serial.println("Relay Controller Initializing...");

  // Initialize relay pins as OUTPUTs and turn them OFF
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  pinMode(RELAY_3_PIN, OUTPUT);
  pinMode(RELAY_4_PIN, OUTPUT);
  digitalWrite(RELAY_1_PIN, LOW);
  digitalWrite(RELAY_2_PIN, LOW);
  digitalWrite(RELAY_3_PIN, LOW);
  digitalWrite(RELAY_4_PIN, LOW);

  // Initialize switch pins as INPUT with pull-up resistors
  pinMode(SWITCH_1_PIN, INPUT_PULLUP);
  pinMode(SWITCH_2_PIN, INPUT_PULLUP);
  pinMode(SWITCH_3_PIN, INPUT_PULLUP);
  pinMode(SWITCH_4_PIN, INPUT_PULLUP);

  // Initialize WiFi in Station mode for ESP-NOW
  WiFi.mode(WIFI_STA);
  Serial.print("Relay Controller MAC: ");
  Serial.println(WiFi.macAddress());

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register callback function for receiving ESP-NOW data
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Relay Controller Ready!");
}

void loop() {
  // Handle switch 1 with debounce and toggle logic
  bool currentSwitch1State = digitalRead(SWITCH_1_PIN);
  if (currentSwitch1State != lastSwitch1State) {
    lastDebounceTime1 = millis();
  }
  if ((millis() - lastDebounceTime1) > DEBOUNCE_DELAY) {
    if (currentSwitch1State == LOW && lastSwitch1State == HIGH) { // Detect switch press (HIGH → LOW transition)
      relay1State = !relay1State; // Toggle relay state
      digitalWrite(RELAY_1_PIN, relay1State);
      Serial.printf("Relay 1 toggled manually to: %s\n", relay1State ? "ON" : "OFF");
    }
  }
  lastSwitch1State = currentSwitch1State;

  // Handle switch 2
  bool currentSwitch2State = digitalRead(SWITCH_2_PIN);
  if (currentSwitch2State != lastSwitch2State) {
    lastDebounceTime2 = millis();
  }
  if ((millis() - lastDebounceTime2) > DEBOUNCE_DELAY) {
    if (currentSwitch2State == LOW && lastSwitch2State == HIGH) {
      relay2State = !relay2State;
      digitalWrite(RELAY_2_PIN, relay2State);
      Serial.printf("Relay 2 toggled manually to: %s\n", relay2State ? "ON" : "OFF");
    }
  }
  lastSwitch2State = currentSwitch2State;

  // Handle switch 3
  bool currentSwitch3State = digitalRead(SWITCH_3_PIN);
  if (currentSwitch3State != lastSwitch3State) {
    lastDebounceTime3 = millis();
  }
  if ((millis() - lastDebounceTime3) > DEBOUNCE_DELAY) {
    if (currentSwitch3State == LOW && lastSwitch3State == HIGH) {
      relay3State = !relay3State;
      digitalWrite(RELAY_3_PIN, relay3State);
      Serial.printf("Relay 3 toggled manually to: %s\n", relay3State ? "ON" : "OFF");
    }
  }
  lastSwitch3State = currentSwitch3State;

  // Handle switch 4
  bool currentSwitch4State = digitalRead(SWITCH_4_PIN);
  if (currentSwitch4State != lastSwitch4State) {
    lastDebounceTime4 = millis();
  }
  if ((millis() - lastDebounceTime4) > DEBOUNCE_DELAY) {
    if (currentSwitch4State == LOW && lastSwitch4State == HIGH) {
      relay4State = !relay4State;
      digitalWrite(RELAY_4_PIN, relay4State);
      Serial.printf("Relay 4 toggled manually to: %s\n", relay4State ? "ON" : "OFF");
    }
  }
  lastSwitch4State = currentSwitch4State;
}
