#define STEP_PIN_1  16
#define DIR_PIN_1   4

#define STEP_PIN_2  15
#define DIR_PIN_2   2

#define STEP_PIN_3  5
#define DIR_PIN_3   17

#define STEP_PIN_4  19
#define DIR_PIN_4   18

#define ENABLE_PIN  23  

#define BUTTON_PIN1 26

void setup() {
    
    pinMode(BUTTON_PIN1, INPUT_PULLUP);  

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

    // กำหนดทิศทางการหมุนของแต่ละมอเตอร์
    digitalWrite(DIR_PIN_1, HIGH);  // CW
    digitalWrite(DIR_PIN_2, LOW);   // CCW
    digitalWrite(DIR_PIN_3, HIGH);  // CW
    digitalWrite(DIR_PIN_4, LOW);   // CCW
}

void loop() {
  bool buttonState = digitalRead(BUTTON_PIN1); 
    if (buttonState == 0) 
    {  
        stepMotor(STEP_PIN_1);
        stepMotor(STEP_PIN_2);
        stepMotor(STEP_PIN_3);
        stepMotor(STEP_PIN_4);
    } 

}

void stepMotor(int stepPin) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(8);  // ปรับความเร็วของมอเตอร์
    digitalWrite(stepPin, LOW);
    delayMicroseconds(8);
}
