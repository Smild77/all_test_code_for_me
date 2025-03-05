#include <Arduino.h>

#define IN1  5  // ขา IN1 ของ L298N
#define IN2  18  // ขา IN2 ของ L298N
#define IN3  19  // ขา IN3 ของ L298N
#define IN4  21  // ขา IN4 ของ L298N

#define BUTTON_PIN1 14
#define BUTTON_PIN2 4 //
#define BUTTON_PIN3 15 //
#define BUTTON_PIN4 17 //


void stopMotor();
void stopTurning();
void turnLeft();
void turnRight();
void forwardDirection();
void reverseDirection();

void setup() {
    Serial.begin(115200);
    
    // ตั้งค่าขาอินพุตสำหรับปุ่มกด
    pinMode(BUTTON_PIN1, INPUT_PULLUP);
    pinMode(BUTTON_PIN2, INPUT_PULLUP);
    pinMode(BUTTON_PIN3, INPUT_PULLUP);
    pinMode(BUTTON_PIN4, INPUT_PULLUP);


    // ตั้งค่าขาเอาต์พุตของ L298N
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
}

void loop() 
{
    // อ่านค่าปุ่มกด

    bool buttonState1 = digitalRead(BUTTON_PIN1);  // เดินหน้า
    bool buttonState2 = digitalRead(BUTTON_PIN2);  // ถอยหลัง
    bool buttonState3 = digitalRead(BUTTON_PIN3);  // เลี้ยวซ้าย
    bool buttonState4 = digitalRead(BUTTON_PIN4);  // เลี้ยวขวา

    // ควบคุมการเดินหน้า-ถอยหลัง
    if (buttonState1 == 1) {
        Serial.println("Forward");
        forwardDirection();
    } 
    else if (buttonState2 == 1) {
        Serial.println("Backward");
        reverseDirection();
    } 
    else {
        stopMotor();  
    }

    // ควบคุมการเลี้ยว
    if (buttonState3 == 1) {
        Serial.println("Turn Left");
        turnLeft();
    } 
    else if (buttonState4 == 1) {
        Serial.println("Turn Right");
        turnRight();
    } 
    else {
        stopTurning();  
    }
}

// ฟังก์ชันควบคุมการเดินหน้า-ถอยหลัง
void forwardDirection() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
}

void reverseDirection() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
}

void stopMotor() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
}

// ฟังก์ชันควบคุมการเลี้ยว
void turnLeft() {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void turnRight() {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}

void stopTurning() {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

