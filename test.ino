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

// Variables to track the relay states
bool relay1State = LOW;
bool relay2State = LOW;
bool relay3State = LOW;
bool relay4State = LOW;

// Variables to track the last switch states
bool lastSwitch1State = HIGH;
bool lastSwitch2State = HIGH;
bool lastSwitch3State = HIGH;
bool lastSwitch4State = HIGH;

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

    // Toggle the relay state based on the received command
    switch (command.relayNumber) {
      case 1:
        relay1State = !relay1State; // Toggle relay state
        digitalWrite(RELAY_1_PIN, relay1State);
        break;
      case 2:
        relay2State = !relay2State; // Toggle relay state
        digitalWrite(RELAY_2_PIN, relay2State);
        break;
      case 3:
        relay3State = !relay3State; // Toggle relay state
        digitalWrite(RELAY_3_PIN, relay3State);
        break;
      case 4:
        relay4State = !relay4State; // Toggle relay state
        digitalWrite(RELAY_4_PIN, relay4State);
        break;
      default:
        Serial.println("Invalid relay number!");
        break;
    }
  } else {
    Serial.println("Invalid data length received!");
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
  // Read switch states and toggle relays on press and release
  bool currentSwitch1State = digitalRead(SWITCH_1_PIN);
  if (currentSwitch1State != lastSwitch1State) {
    delay(50); // Simple debounce delay
    if (currentSwitch1State == LOW || currentSwitch1State == HIGH) { // Toggle on both press and release
      relay1State = !relay1State; // Toggle relay state
      digitalWrite(RELAY_1_PIN, relay1State);
      Serial.println("Switch 1 toggled: Relay 1 state changed.");
    }
    lastSwitch1State = currentSwitch1State;
  }

  bool currentSwitch2State = digitalRead(SWITCH_2_PIN);
  if (currentSwitch2State != lastSwitch2State) {
    delay(50); // Simple debounce delay
    if (currentSwitch2State == LOW || currentSwitch2State == HIGH) { // Toggle on both press and release
      relay2State = !relay2State; // Toggle relay state
      digitalWrite(RELAY_2_PIN, relay2State);
      Serial.println("Switch 2 toggled: Relay 2 state changed.");
    }
    lastSwitch2State = currentSwitch2State;
  }

  bool currentSwitch3State = digitalRead(SWITCH_3_PIN);
  if (currentSwitch3State != lastSwitch3State) {
    delay(50); // Simple debounce delay
    if (currentSwitch3State == LOW || currentSwitch3State == HIGH) { // Toggle on both press and release
      relay3State = !relay3State; // Toggle relay state
      digitalWrite(RELAY_3_PIN, relay3State);
      Serial.println("Switch 3 toggled: Relay 3 state changed.");
    }
    lastSwitch3State = currentSwitch3State;
  }

  bool currentSwitch4State = digitalRead(SWITCH_4_PIN);
  if (currentSwitch4State != lastSwitch4State) {
    delay(50); // Simple debounce delay
    if (currentSwitch4State == LOW || currentSwitch4State == HIGH) { // Toggle on both press and release
      relay4State = !relay4State; // Toggle relay state
      digitalWrite(RELAY_4_PIN, relay4State);
      Serial.println("Switch 4 toggled: Relay 4 state changed.");
    }
    lastSwitch4State = currentSwitch4State;
  }
}
