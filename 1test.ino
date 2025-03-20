void setup() {
  Serial.begin(115200);
  pinMode(3, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(5, OUTPUT);

  Serial.println("Turning Relays ON for 2 seconds...");
  digitalWrite(3, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(19, HIGH);
  digitalWrite(5, HIGH);
  delay(2000);

  Serial.println("Turning Relays OFF...");
  digitalWrite(3, LOW);
  digitalWrite(10, LOW);
  digitalWrite(19, LOW);
  digitalWrite(5, LOW);
}

void loop() { }
