#include <esp_now.h>
#include <WiFi.h>

// Define relay pins
#define RELAY_1_PIN 3
#define RELAY_2_PIN 10
#define RELAY_3_PIN 19
#define RELAY_4_PIN 5

// Define switch pins (active low)
#define SWITCH_1_PIN 6
#define SWITCH_2_PIN 4
#define SWITCH_3_PIN 18
#define SWITCH_4_PIN 9

// Hardcoded MAC address of the gateway
uint8_t gatewayMacAddress[] = {0x24, 0x6F, 0x28, 0x01, 0x02, 0x03};

// Structure to receive data
typedef struct {
  uint8_t relayNumber; // Relay number (1, 2, 3, or 4)
  uint8_t state;       // 0 = OFF, 1 = ON
} RelayCommand;

// Function to control relays
void controlRelay(uint8_t relayNumber, uint8_t state) {
  switch (relayNumber) {
    case 1:
      digitalWrite(RELAY_1_PIN, state);
      break;
    case 2:
      digitalWrite(RELAY_2_PIN, state);
      break;
    case 3:
      digitalWrite(RELAY_3_PIN, state);
      break;
    case 4:
      digitalWrite(RELAY_4_PIN, state);
      break;
    default:
      Serial.println("Invalid relay number!");
      break;
  }
}

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

    // Control the relay
    controlRelay(command.relayNumber, command.state);
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

  // Initialize switch pins (active low, so use INPUT_PULLUP)
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
  // Check the state of each switch and control the relays accordingly
  if (digitalRead(SWITCH_1_PIN) == LOW) { // Active low
    controlRelay(1, HIGH); // Turn on Relay 1
  } else {
    controlRelay(1, LOW); // Turn off Relay 1
  }

  if (digitalRead(SWITCH_2_PIN) == LOW) { // Active low
    controlRelay(2, HIGH); // Turn on Relay 2
  } else {
    controlRelay(2, LOW); // Turn off Relay 2
  }

  if (digitalRead(SWITCH_3_PIN) == LOW) { // Active low
    controlRelay(3, HIGH); // Turn on Relay 3
  } else {
    controlRelay(3, LOW); // Turn off Relay 3
  }

  if (digitalRead(SWITCH_4_PIN) == LOW) { // Active low
    controlRelay(4, HIGH); // Turn on Relay 4
  } else {
    controlRelay(4, LOW); // Turn off Relay 4
  }

  // Small delay to debounce switches
  delay(100);
}
