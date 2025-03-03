#include <Arduino.h>
#include <Wire.h>

#define SLAVE_ADDRESS 0x04
#define BUTTON_PIN 15

void requestEvent();

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);
}

void loop() {
  // Nothing to do here
}

void requestEvent() {
  int buttonState = digitalRead(BUTTON_PIN);
  Wire.write(buttonState);  // ส่งสถานะของปุ่ม
}
