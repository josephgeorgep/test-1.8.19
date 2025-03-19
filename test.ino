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

// Hardcoded MAC address of the gateway
uint8_t gatewayMacAddress[] = {0x34, 0x85, 0x18, 0x17, 0x6B, 0x50};

// Structure to receive data
typedef struct {
  uint8_t relayNumber; // Relay number (1, 2, 3, or 4)
  uint8_t state;       // 0 = OFF, 1 = ON
} RelayCommand;

// Variables to track the relay states
bool relay1State = LOW;
bool relay2State = LOW;
bool relay3State = LOW;
bool relay4State = LOW;

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
  WiFi.channel(1); // Set WiFi channel to 1
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
  // Read switch states and control relays
  if (digitalRead(SWITCH_1_PIN) == LOW) { // Switch 1 pressed (active low)
    relay1State = HIGH; // Turn on Relay 1
    digitalWrite(RELAY_1_PIN, relay1State);
    delay(500); // Debounce delay
    while (digitalRead(SWITCH_1_PIN) == LOW); // Wait for switch release
  } else {
    relay1State = LOW; // Turn off Relay 1
    digitalWrite(RELAY_1_PIN, relay1State);
  }

  if (digitalRead(SWITCH_2_PIN) == LOW) { // Switch 2 pressed (active low)
    relay2State = HIGH; // Turn on Relay 2
    digitalWrite(RELAY_2_PIN, relay2State);
    delay(500); // Debounce delay
    while (digitalRead(SWITCH_2_PIN) == LOW); // Wait for switch release
  } else {
    relay2State = LOW; // Turn off Relay 2
    digitalWrite(RELAY_2_PIN, relay2State);
  }

  if (digitalRead(SWITCH_3_PIN) == LOW) { // Switch 3 pressed (active low)
    relay3State = HIGH; // Turn on Relay 3
    digitalWrite(RELAY_3_PIN, relay3State);
    delay(500); // Debounce delay
    while (digitalRead(SWITCH_3_PIN) == LOW); // Wait for switch release
  } else {
    relay3State = LOW; // Turn off Relay 3
    digitalWrite(RELAY_3_PIN, relay3State);
  }

  if (digitalRead(SWITCH_4_PIN) == LOW) { // Switch 4 pressed (active low)
    relay4State = HIGH; // Turn on Relay 4
    digitalWrite(RELAY_4_PIN, relay4State);
    delay(500); // Debounce delay
    while (digitalRead(SWITCH_4_PIN) == LOW); // Wait for switch release
  } else {
    relay4State = LOW; // Turn off Relay 4
    digitalWrite(RELAY_4_PIN, relay4State);
  }
}
