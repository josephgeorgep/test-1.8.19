void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    RelayCommand command;
    memcpy(&command, incomingData, sizeof(command));

    Serial.printf("Received: Relay %d -> %s\n", command.relayNumber, command.state ? "ON" : "OFF");

    switch (command.relayNumber) {
        case 1: digitalWrite(RELAY_1_PIN, command.state); break;
        case 2: digitalWrite(RELAY_2_PIN, command.state); break;
        case 3: digitalWrite(RELAY_3_PIN, command.state); break;
        case 4: digitalWrite(RELAY_4_PIN, command.state); break;
        default: Serial.println("Invalid relay number!"); return;
    }

    // Verify state change
    Serial.printf("Relay %d actual state: %d\n", command.relayNumber, digitalRead(command.relayNumber));
}
