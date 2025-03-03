#include <Arduino.h>

#define STEP_PIN_1  16
#define DIR_PIN_1   4

#define STEP_PIN_2  15
#define DIR_PIN_2   2

#define STEP_PIN_3  5
#define DIR_PIN_3   17

#define STEP_PIN_4  19
#define DIR_PIN_4   18

#define ENABLE_PIN  23  

#define BUTTON_PIN1 26  // ปุ่มสำหรับเดินหน้า
#define BUTTON_PIN2 25  // ปุ่มสำหรับถอยหลัง
#define BUTTON_PIN3 32  // ปุ่มสำหรับเลี้ยวซ้าย
#define BUTTON_PIN4 33  // ปุ่มสำหรับเลี้ยวขวา

void stepMotor(int stepPin, int delayTime);
void forwardDirection();
void reverseDirection();
void turnLeft();
void turnRight();

void setup() {
    pinMode(BUTTON_PIN1, INPUT_PULLUP);
    pinMode(BUTTON_PIN2, INPUT_PULLUP);
    pinMode(BUTTON_PIN3, INPUT_PULLUP);
    pinMode(BUTTON_PIN4, INPUT_PULLUP);

    pinMode(STEP_PIN_1, OUTPUT);
    pinMode(DIR_PIN_1, OUTPUT);
    
    pinMode(STEP_PIN_2, OUTPUT);
    pinMode(DIR_PIN_2, OUTPUT);
    
    pinMode(STEP_PIN_3, OUTPUT);
    pinMode(DIR_PIN_3, OUTPUT);
    
    pinMode(STEP_PIN_4, OUTPUT);
    pinMode(DIR_PIN_4, OUTPUT);
    
    pinMode(ENABLE_PIN, OUTPUT);
    digitalWrite(ENABLE_PIN, LOW);  // เปิดใช้งานมอเตอร์
}

void loop() {
    bool buttonState1 = digitalRead(BUTTON_PIN1);  // ปุ่มเดินหน้า
    bool buttonState2 = digitalRead(BUTTON_PIN2);  // ปุ่มถอยหลัง
    bool buttonState3 = digitalRead(BUTTON_PIN3);  // ปุ่มเลี้ยวซ้าย
    bool buttonState4 = digitalRead(BUTTON_PIN4);  // ปุ่มเลี้ยวขวา
    
    if (buttonState1 == 0) {
        forwardDirection();
        digitalWrite(ENABLE_PIN, LOW);
        stepMotor(STEP_PIN_1, 6); // ความเร็วเดินหน้า
        stepMotor(STEP_PIN_2, 6); // ความเร็วเดินหน้า
        stepMotor(STEP_PIN_3, 6); // ความเร็วเดินหน้า
        stepMotor(STEP_PIN_4, 6); // ความเร็วเดินหน้า
    } 
    else if (buttonState2 == 0) {
        reverseDirection();
        digitalWrite(ENABLE_PIN, LOW);
        stepMotor(STEP_PIN_1, 6); // ความเร็วถอยหลัง
        stepMotor(STEP_PIN_2, 6); // ความเร็วถอยหลัง
        stepMotor(STEP_PIN_3, 6); // ความเร็วถอยหลัง
        stepMotor(STEP_PIN_4, 6); // ความเร็วถอยหลัง
    } 
    else if (buttonState3 == 0) {
        turnLeft();
        digitalWrite(ENABLE_PIN, LOW);
        stepMotor(STEP_PIN_1, 50); // ความเร็วเลี้ยวซ้าย
        stepMotor(STEP_PIN_2, 50); // ความเร็วเลี้ยวซ้าย
        stepMotor(STEP_PIN_3, 50); // ความเร็วเลี้ยวซ้าย
        stepMotor(STEP_PIN_4, 50); // ความเร็วเลี้ยวซ้าย
    } 
    else if (buttonState4 == 0) {
        turnRight();
        digitalWrite(ENABLE_PIN, LOW);
        stepMotor(STEP_PIN_1, 50); // ความเร็วเลี้ยวขวา
        stepMotor(STEP_PIN_2, 50); // ความเร็วเลี้ยวขวา
        stepMotor(STEP_PIN_3, 50); // ความเร็วเลี้ยวขวา
        stepMotor(STEP_PIN_4, 50); // ความเร็วเลี้ยวขวา
    } 
    else {
        digitalWrite(ENABLE_PIN, HIGH);  // ปิดมอเตอร์ถ้าไม่กดปุ่มใดๆ
    }
}

void stepMotor(int stepPin, int delayTime) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayTime);  // ใช้ค่าความเร็วตามที่กำหนด
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delayTime);  // ใช้ค่าความเร็วตามที่กำหนด
}

void forwardDirection() {
  digitalWrite(DIR_PIN_1, HIGH);  // CW
  digitalWrite(DIR_PIN_2, LOW);   // CCW
  digitalWrite(DIR_PIN_3, HIGH);  // CW
  digitalWrite(DIR_PIN_4, LOW);   // CCW
}

void reverseDirection() {
  digitalWrite(DIR_PIN_1, LOW);   // CCW
  digitalWrite(DIR_PIN_2, HIGH);  // CW
  digitalWrite(DIR_PIN_3, LOW);   // CCW
  digitalWrite(DIR_PIN_4, HIGH);  // CW
}

void turnLeft() {
  digitalWrite(DIR_PIN_2, HIGH);
  digitalWrite(DIR_PIN_4, HIGH);
}

void turnRight() {
  digitalWrite(DIR_PIN_1, LOW);
  digitalWrite(DIR_PIN_3, LOW);
}
