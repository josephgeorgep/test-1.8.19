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

// Relay states
bool relayStates[4] = {0, 0, 0, 0}; 

// Structure to receive data
typedef struct {
  uint8_t relayNumber;
  uint8_t state;
} RelayCommand;

// Callback when ESPNOW data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (len != sizeof(RelayCommand)) {
    Serial.println("Invalid ESPNOW data received!");
    return;
  }

  RelayCommand command;
  memcpy(&command, incomingData, sizeof(command));

  if (command.relayNumber < 1 || command.relayNumber > 4) {
    Serial.println("Invalid relay number!");
    return;
  }

  int relayIndex = command.relayNumber - 1;
  relayStates[relayIndex] = command.state;
  digitalWrite(RELAY_1_PIN + relayIndex, relayStates[relayIndex]);

  Serial.printf("ESPNOW: Relay %d -> %s\n", command.relayNumber, relayStates[relayIndex] ? "ON" : "OFF");
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

  // Initialize switch pins
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

  Serial.println("ESP-NOW & Manual Switches Ready!");
}

void loop() {
  // Check switches and toggle relays
  static bool lastSwitchStates[4] = {1, 1, 1, 1};

  int switchPins[4] = {SWITCH_1_PIN, SWITCH_2_PIN, SWITCH_3_PIN, SWITCH_4_PIN};
  int relayPins[4] = {RELAY_1_PIN, RELAY_2_PIN, RELAY_3_PIN, RELAY_4_PIN};

  for (int i = 0; i < 4; i++) {
    bool currentState = digitalRead(switchPins[i]);

    if (currentState == LOW && lastSwitchStates[i] == HIGH) {  // Detect press
      relayStates[i] = !relayStates[i];  // Toggle relay state
      digitalWrite(relayPins[i], relayStates[i]);
      Serial.printf("Switch %d toggled: Relay %d is now %s\n", i + 1, i + 1, relayStates[i] ? "ON" : "OFF");
      delay(300);  // Debounce delay
    }
    
    lastSwitchStates[i] = currentState;
  }
}
