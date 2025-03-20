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

void setup() {
  // Initialize Serial for logging
  Serial.begin(115200);
  Serial.println("Testing Manual Switches...");

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
}

void loop() {
  // Read switch states and control relays
  if (digitalRead(SWITCH_1_PIN) == LOW) { // Switch 1 pressed (active low)
    digitalWrite(RELAY_1_PIN, HIGH); // Turn on Relay 1
    Serial.println("Switch 1 pressed: Relay 1 ON");
  } else {
    digitalWrite(RELAY_1_PIN, LOW); // Turn off Relay 1
    Serial.println("Switch 1 released: Relay 1 OFF");
  }

  if (digitalRead(SWITCH_2_PIN) == LOW) { // Switch 2 pressed (active low)
    digitalWrite(RELAY_2_PIN, HIGH); // Turn on Relay 2
    Serial.println("Switch 2 pressed: Relay 2 ON");
  } else {
    digitalWrite(RELAY_2_PIN, LOW); // Turn off Relay 2
    Serial.println("Switch 2 released: Relay 2 OFF");
  }

  if (digitalRead(SWITCH_3_PIN) == LOW) { // Switch 3 pressed (active low)
    digitalWrite(RELAY_3_PIN, HIGH); // Turn on Relay 3
    Serial.println("Switch 3 pressed: Relay 3 ON");
  } else {
    digitalWrite(RELAY_3_PIN, LOW); // Turn off Relay 3
    Serial.println("Switch 3 released: Relay 3 OFF");
  }

  if (digitalRead(SWITCH_4_PIN) == LOW) { // Switch 4 pressed (active low)
    digitalWrite(RELAY_4_PIN, HIGH); // Turn on Relay 4
    Serial.println("Switch 4 pressed: Relay 4 ON");
  } else {
    digitalWrite(RELAY_4_PIN, LOW); // Turn off Relay 4
    Serial.println("Switch 4 released: Relay 4 OFF");
  }

  delay(100); // Small delay for stability
}