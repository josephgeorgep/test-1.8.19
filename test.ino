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

// Debounce delay (in milliseconds)
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
  // Read switch states with debouncing
  bool currentSwitch1State = digitalRead(SWITCH_1_PIN);
  if (currentSwitch1State != lastSwitch1State) {
    lastDebounceTime1 = millis(); // Reset the debounce timer
  }
  if ((millis() - lastDebounceTime1) > DEBOUNCE_DELAY) {
    // Check if the switch state has changed
    if (currentSwitch1State == LOW && lastSwitch1State == HIGH) { // Falling edge (switch pressed)
      relay1State = !relay1State; // Toggle relay state
      digitalWrite(RELAY_1_PIN, relay1State);
      Serial.println("Switch 1 pressed: Relay 1 toggled.");
    }
    lastSwitch1State = currentSwitch1State;
  }

  bool currentSwitch2State = digitalRead(SWITCH_2_PIN);
  if (currentSwitch2State != lastSwitch2State) {
    lastDebounceTime2 = millis(); // Reset the debounce timer
  }
  if ((millis() - lastDebounceTime2) > DEBOUNCE_DELAY) {
    // Check if the switch state has changed
    if (currentSwitch2State == LOW && lastSwitch2State == HIGH) { // Falling edge (switch pressed)
      relay2State = !relay2State; // Toggle relay state
      digitalWrite(RELAY_2_PIN, relay2State);
      Serial.println("Switch 2 pressed: Relay 2 toggled.");
    }
    lastSwitch2State = currentSwitch2State;
  }

  bool currentSwitch3State = digitalRead(SWITCH_3_PIN);
  if (currentSwitch3State != lastSwitch3State) {
    lastDebounceTime3 = millis(); // Reset the debounce timer
  }
  if ((millis() - lastDebounceTime3) > DEBOUNCE_DELAY) {
    // Check if the switch state has changed
    if (currentSwitch3State == LOW && lastSwitch3State == HIGH) { // Falling edge (switch pressed)
      relay3State = !relay3State; // Toggle relay state
      digitalWrite(RELAY_3_PIN, relay3State);
      Serial.println("Switch 3 pressed: Relay 3 toggled.");
    }
    lastSwitch3State = currentSwitch3State;
  }

  bool currentSwitch4State = digitalRead(SWITCH_4_PIN);
  if (currentSwitch4State != lastSwitch4State) {
    lastDebounceTime4 = millis(); // Reset the debounce timer
  }
  if ((millis() - lastDebounceTime4) > DEBOUNCE_DELAY) {
    // Check if the switch state has changed
    if (currentSwitch4State == LOW && lastSwitch4State == HIGH) { // Falling edge (switch pressed)
      relay4State = !relay4State; // Toggle relay state
      digitalWrite(RELAY_4_PIN, relay4State);
      Serial.println("Switch 4 pressed: Relay 4 toggled.");
    }
    lastSwitch4State = currentSwitch4State;
  }

  // Debug logs for switch states
  Serial.print("Switch States: ");
  Serial.print("S1=");
  Serial.print(digitalRead(SWITCH_1_PIN));
  Serial.print(" S2=");
  Serial.print(digitalRead(SWITCH_2_PIN));
  Serial.print(" S3=");
  Serial.print(digitalRead(SWITCH_3_PIN));
  Serial.print(" S4=");
  Serial.println(digitalRead(SWITCH_4_PIN));

  delay(100); // Small delay to avoid flooding the Serial Monitor
}
