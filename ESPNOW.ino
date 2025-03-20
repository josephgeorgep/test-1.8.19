#include <esp_now.h>
#include <WiFi.h>

// Define relay pins
#define RELAY_1_PIN 3
#define RELAY_2_PIN 10
#define RELAY_3_PIN 19
#define RELAY_4_PIN 5

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

    // Control the relay
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
  Serial.println("Testing ESP-NOW...");

  // Initialize relay pins
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  pinMode(RELAY_3_PIN, OUTPUT);
  pinMode(RELAY_4_PIN, OUTPUT);
  digitalWrite(RELAY_1_PIN, LOW);
  digitalWrite(RELAY_2_PIN, LOW);
  digitalWrite(RELAY_3_PIN, LOW);
  digitalWrite(RELAY_4_PIN, LOW);

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

  Serial.println("ESP-NOW Ready!");
}

void loop() {
}