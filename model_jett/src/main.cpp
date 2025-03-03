#include <Arduino.h>

#define DIR_PIN 7
#define STEP_PIN 11
#define ENABLE_PIN 15


#define proxPin1 46
#define proxPin2 47


bool isRunning = false;
bool direction = true;
String lastCommand = "";


void stopAllMotors();
void rotateUntilProx1();
void rotateUntilProx2();
void handleSerialCommand();

void controlMotor(int motorIndex, bool state, bool dir) {
    isRunning = state;
    digitalWrite(ENABLE_PIN, !state);  
    digitalWrite(DIR_PIN, dir ? HIGH : LOW);

    if (state) {
        digitalWrite(STEP_PIN, HIGH);
        delayMicroseconds(350);  
        digitalWrite(STEP_PIN, LOW);
        delayMicroseconds(350);
    }
}

void setup() {
    pinMode(DIR_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(ENABLE_PIN, OUTPUT);
    pinMode(proxPin1, INPUT);
    pinMode(proxPin2, INPUT);
    digitalWrite(ENABLE_PIN, LOW);
    Serial.begin(9600);
}

void loop() {
    // Serial.println(isRunning);
    if (Serial.available()) {
        handleSerialCommand();
    }
    if (!isRunning) {
        stopAllMotors();
    }
}

void handleSerialCommand() {
    String command = "";
    if (Serial.available()) {
        command = Serial.readStringUntil('\n');
        command.trim();
    }
    if (command.length() > 0) {
        Serial.print("ACK: ");
        Serial.println(command);
    }
    if (command == "s") {
        isRunning = true;
        Serial.println("1111111111");
        controlMotor(0, true, direction);
    } 
    else if (command == "p") {
        isRunning = false;
        Serial.println("0000000");
        stopAllMotors();
    } 
    else if (command == "change direction") {
        direction = !direction; 
        Serial.println("RRRRRRRRR");
    } 
    else if (command == "a") {
        Serial.println("S_proxPin1");
        rotateUntilProx1();
    }
    else if (command == "b") {
        Serial.println("S_proxPin2");
        rotateUntilProx2();
    }
    else {
        Serial.println("ERROR");
    }
    lastCommand = command;
}

void rotateUntilProx1() {
    isRunning = true;
    while (digitalRead(proxPin1) == HIGH) 
    { 
        controlMotor(0, true, direction); 
    }
}

void rotateUntilProx2() {
    isRunning = true;
    while (digitalRead(proxPin2) == HIGH) 
    {
        controlMotor(0, true, !direction); 
    }
}

void stopAllMotors() {
    isRunning = false;
    digitalWrite(ENABLE_PIN, HIGH);
    digitalWrite(STEP_PIN, LOW);
}
