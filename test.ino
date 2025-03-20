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
volatile bool relay1State = LOW;
volatile bool relay2State = LOW;
volatile bool relay3State = LOW;
volatile bool relay4State = LOW;

// ESPNOW Command Structure
typedef struct {
  uint8_t relayNumber;
  uint8_t state;
} RelayCommand;

// ESPNOW Callback
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (len == sizeof(RelayCommand)) {
    RelayCommand command;
    memcpy(&command, incomingData, sizeof(command));

    switch (command.relayNumber) {
      case 1: relay1State = command.state; digitalWrite(RELAY_1_PIN, relay1State); break;
      case 2: relay2State = command.state; digitalWrite(RELAY_2_PIN, relay2State); break;
      case 3: relay3State = command.state; digitalWrite(RELAY_3_PIN, relay3State); break;
      case 4: relay4State = command.state; digitalWrite(RELAY_4_PIN, relay4State); break;
    }

    Serial.printf("Relay %d set to %s via ESPNOW\n", command.relayNumber, command.state ? "ON" : "OFF");
  }
}

// Switch Interrupt Handlers (Toggle Relays)
void IRAM_ATTR toggleRelay1() { relay1State = !relay1State; digitalWrite(RELAY_1_PIN, relay1State); }
void IRAM_ATTR toggleRelay2() { relay2State = !relay2State; digitalWrite(RELAY_2_PIN, relay2State); }
void IRAM_ATTR toggleRelay3() { relay3State = !relay3State; digitalWrite(RELAY_3_PIN, relay3State); }
void IRAM_ATTR toggleRelay4() { relay4State = !relay4State; digitalWrite(RELAY_4_PIN, relay4State); }

void setup() {
  Serial.begin(115200);

  // Initialize Relays
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  pinMode(RELAY_3_PIN, OUTPUT);
  pinMode(RELAY_4_PIN, OUTPUT);
  digitalWrite(RELAY_1_PIN, LOW);
  digitalWrite(RELAY_2_PIN, LOW);
  digitalWrite(RELAY_3_PIN, LOW);
  digitalWrite(RELAY_4_PIN, LOW);

  // Initialize Switches with Interrupts
  pinMode(SWITCH_1_PIN, INPUT_PULLUP);
  pinMode(SWITCH_2_PIN, INPUT_PULLUP);
  pinMode(SWITCH_3_PIN, INPUT_PULLUP);
  pinMode(SWITCH_4_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(SWITCH_1_PIN), toggleRelay1, FALLING);
  attachInterrupt(digitalPinToInterrupt(SWITCH_2_PIN), toggleRelay2, FALLING);
  attachInterrupt(digitalPinToInterrupt(SWITCH_3_PIN), toggleRelay3, FALLING);
  attachInterrupt(digitalPinToInterrupt(SWITCH_4_PIN), toggleRelay4, FALLING);

  // Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("System Ready!");
}

void loop() {
  // Nothing needed in loop() - handled by interrupts and ESP-NOW
}
