#include <esp_now.h>
#include <WiFi.h>

// Define relay pins
#define RELAY_1_PIN 3
#define RELAY_2_PIN 10
#define RELAY_3_PIN 19
#define RELAY_4_PIN 5

// Hardcoded MAC address of the gateway (SECOND C3)
uint8_t gatewayMacAddress[] = {0x34, 0x85, 0x18, 0x17, 0x7D, 0x30}; // Replace with actual MAC

// Structure to match SECOND C3's messaging format
typedef struct {
  int relay_id; // Relay number (0, 1, 2, 3)
  bool state;   // 0 = OFF, 1 = ON
} RelayStatusMessage;

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  Serial.println("[ESP-NOW] ✅ Data received!");

  if (len != sizeof(RelayStatusMessage)) {
    Serial.println("[ERROR] ❌ Invalid data length received!");
    return;
  }

  // Copy received data into the struct
  RelayStatusMessage command;
  memcpy(&command, incomingData, sizeof(command));

  // Debug log: Print received MAC address
  Serial.printf("[ESP-NOW] Received data from %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  // Debug log: Print received relay command
  Serial.printf("[ESP-NOW] 🔄 Relay %d set to %s\n", command.relay_id, command.state ? "ON" : "OFF");

  // Validate relay ID and trigger the correct relay
  switch (command.relay_id) {
    case 0:
      digitalWrite(RELAY_1_PIN, command.state);
      break;
    case 1:
      digitalWrite(RELAY_2_PIN, command.state);
      break;
    case 2:
      digitalWrite(RELAY_3_PIN, command.state);
      break;
    case 3:
      digitalWrite(RELAY_4_PIN, command.state);
      break;
    default:
      Serial.println("[ERROR] ❌ Invalid relay ID received!");
      return;
  }

  // Confirm relay state after setting
  Serial.printf("[ESP-NOW] ✅ Relay %d toggled successfully.\n", command.relay_id);
}

void setup() {
  Serial.begin(115200);
  Serial.println("[SETUP] Initializing FIRST C3...");

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
  Serial.print("[WiFi] ✅ FIRST C3 MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("[ERROR] ❌ ESP-NOW Initialization Failed!");
    return;
  }

  Serial.println("[ESP-NOW] ✅ Initialized Successfully.");

  // Register ESP-NOW receive callback
  if (esp_now_register_recv_cb(OnDataRecv) == ESP_OK) {
    Serial.println("[ESP-NOW] ✅ Receive Callback Registered Successfully.");
  } else {
    Serial.println("[ERROR] ❌ Failed to Register Receive Callback!");
  }

  Serial.println("[SETUP] ✅ FIRST C3 Ready.");
}

void loop() {
  // Nothing to do here, everything handled via ESP-NOW callback
}
