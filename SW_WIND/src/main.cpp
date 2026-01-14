#include <Arduino.h>

const int RELAYS[] = {16, 14, 12, 13};
const int NUM_RELAYS = sizeof(RELAYS) / sizeof(RELAYS[0]);

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NUM_RELAYS; i++) {
    pinMode(RELAYS[i], OUTPUT);
    digitalWrite(RELAYS[i], LOW);
  }
  Serial.println("Ready. Send a, b, c, d to turn ON relay 1-4, or s to turn OFF all.");
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();

    switch (command) {
      case 'a':
        digitalWrite(RELAYS[0], HIGH);
        Serial.println("Relay 1 ON");
        break;
      case 'b':
        digitalWrite(RELAYS[1], HIGH);
        Serial.println("Relay 2 ON");
        break;
      case 'c':
        digitalWrite(RELAYS[2], HIGH);
        Serial.println("Relay 3 ON");
        break;
      case 'd':
        digitalWrite(RELAYS[3], HIGH);
        Serial.println("Relay 4 ON");
        break;
      case 's':
        for (int i = 0; i < NUM_RELAYS; i++) {
          digitalWrite(RELAYS[i], LOW);
        }
        Serial.println("All relays OFF");
        break;
      default:
        Serial.println("Unknown command");
        break;
    }
  }
}
