#include <esp_now.h>
#include <WiFi.h>

#define RELAY_COUNT 4

// **Updated GPIO Assignments**
const int relayPins[RELAY_COUNT] = {3, 10, 19, 5};  // GPIOs for relays

typedef struct {
  int relay_id;
  bool state;
} RelayStatusMessage;

uint8_t gatewayMac[] = {0x24, 0x6F, 0x28, 0xXX, 0xXX, 0xXX};  // Replace with SECOND C3's MAC address

// **ESP-NOW receive function**
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  Serial.println("[ESP-NOW] ✅ Data received!");

  RelayStatusMessage msg;
  memcpy(&msg, incomingData, sizeof(msg));

  Serial.print("[ESP-NOW] 🔄 Received Command -> Relay ID: ");
  Serial.print(msg.relay_id);
  Serial.print(" State: ");
  Serial.println(msg.state ? "ON" : "OFF");

  // Ensure valid relay ID
  if (msg.relay_id >= 0 && msg.relay_id < RELAY_COUNT) {
    Serial.print("[ESP-NOW] ✅ Toggling Relay ");
    Serial.print(msg.relay_id);
    Serial.print(" to ");
    Serial.println(msg.state ? "ON" : "OFF");

    digitalWrite(relayPins[msg.relay_id], msg.state ? HIGH : LOW);

    // Read back the GPIO state to confirm
    int pinState = digitalRead(relayPins[msg.relay_id]);
    Serial.print("[DEBUG] 🔍 GPIO ");
    Serial.print(relayPins[msg.relay_id]);
    Serial.print(" state: ");
    Serial.println(pinState == HIGH ? "ON" : "OFF");

    Serial.println("[ESP-NOW] ✅ Relay toggled successfully.");
  } else {
    Serial.println("[ESP-NOW] ❌ Invalid Relay ID received!");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("[SETUP] Initializing FIRST C3...");

  WiFi.mode(WIFI_STA);  // Required for ESP-NOW
  Serial.println("[WiFi] ✅ Set to Station Mode");

  if (esp_now_init() != ESP_OK) {
    Serial.println("[ERROR] ❌ ESP-NOW Initialization Failed!");
    return;
  }

  Serial.println("[ESP-NOW] ✅ Initialized Successfully.");
  
  // Register ESP-NOW receive callback
  esp_now_register_recv_cb(onDataRecv);

  // Initialize relays
  for (int i = 0; i < RELAY_COUNT; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW);  // Ensure relays start OFF

    Serial.print("[SETUP] ✅ Configured Relay ");
    Serial.print(i);
    Serial.print(" -> GPIO ");
    Serial.println(relayPins[i]);
  }

  Serial.println("[SETUP] ✅ FIRST C3 Ready.");
}

void loop() {
  // Everything is handled via ESP-NOW
}
