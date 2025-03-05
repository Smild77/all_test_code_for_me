#define STEP_PIN_1  16
#define DIR_PIN_1   4

#define STEP_PIN_2  15
#define DIR_PIN_2   2

#define STEP_PIN_3  5
#define DIR_PIN_3   17

#define STEP_PIN_4  19
#define DIR_PIN_4   18

#define ENABLE_PIN  23  

const int IN1 = 12;  // ขา IN1 ของ L298N
const int IN2 = 13;  // ขา IN2 ของ L298N
const int IN3 = 22;  // ขา IN3 ของ L298N
const int IN4 = 21;  // ขา IN4 ของ L298N

#define BUTTON_PIN1 25  
#define BUTTON_PIN2 26
#define BUTTON_PIN3 32  
#define BUTTON_PIN4 33

#define UP_BUTTON_PIN 14 
#define DOWN_BUTTON_PIN 27


void moveUp();
void moveDown();
void turnLeft();
void stopMotor();
void turnRight();
void forwardDirection();
void reverseDirection();
void stepMotor(int stepPin, int delayTime);

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN1, INPUT_PULLUP);
    pinMode(BUTTON_PIN2, INPUT_PULLUP);
    pinMode(BUTTON_PIN3, INPUT_PULLUP);
    pinMode(BUTTON_PIN4, INPUT_PULLUP);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(UP_BUTTON_PIN, INPUT_PULLUP);  
    pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP); 

    pinMode(STEP_PIN_1, OUTPUT);
    pinMode(DIR_PIN_1, OUTPUT);
    
    pinMode(STEP_PIN_2, OUTPUT);
    pinMode(DIR_PIN_2, OUTPUT);
    
    pinMode(STEP_PIN_3, OUTPUT);
    pinMode(DIR_PIN_3, OUTPUT);
    
    pinMode(STEP_PIN_4, OUTPUT);
    pinMode(DIR_PIN_4, OUTPUT);
    
    pinMode(ENABLE_PIN, OUTPUT);
    digitalWrite(ENABLE_PIN, LOW);  
}

void loop() 
{
    //มอเตอร์เกียร์
    bool downState = digitalRead(UP_BUTTON_PIN);
    bool upState = digitalRead(DOWN_BUTTON_PIN);

    if (upState == 0 ) 
    {
      Serial.println("UP");
      moveUp();  
    } 
    else if (downState == 0)  
    {
      Serial.println("DOWN");
      moveDown(); 
    } 
    else 
    {
      stopMotor();  
    }

    //ควบคุมล้อ
    bool buttonState1 = digitalRead(BUTTON_PIN1);  
    bool buttonState2 = digitalRead(BUTTON_PIN2); 
    bool buttonState3 = digitalRead(BUTTON_PIN3);  
    bool buttonState4 = digitalRead(BUTTON_PIN4);

    if (buttonState2 == 0) 
    {
        reverseDirection();
        digitalWrite(ENABLE_PIN, LOW);
        stepMotor(STEP_PIN_1, 15); 
        stepMotor(STEP_PIN_2, 15); 
        stepMotor(STEP_PIN_3, 15); 
        stepMotor(STEP_PIN_4, 15); 
    } 
    else if (buttonState1 == 0) 
    {
        forwardDirection();
        digitalWrite(ENABLE_PIN, LOW);
        stepMotor(STEP_PIN_1, 15); 
        stepMotor(STEP_PIN_2, 15); 
        stepMotor(STEP_PIN_3, 15); 
        stepMotor(STEP_PIN_4, 15); 
    } 
    else if (buttonState3 == 0) 
    {
        turnLeft();
        digitalWrite(ENABLE_PIN, LOW);
        stepMotor(STEP_PIN_1, 30); 
        stepMotor(STEP_PIN_2, 30); 
        stepMotor(STEP_PIN_3, 30); 
        stepMotor(STEP_PIN_4, 30); 
    } 
    else if (buttonState4 == 0) 
    {
        turnRight();
        digitalWrite(ENABLE_PIN, LOW);
        stepMotor(STEP_PIN_1, 30); 
        stepMotor(STEP_PIN_2, 30); 
        stepMotor(STEP_PIN_3, 30); 
        stepMotor(STEP_PIN_4, 30); 
    }  
    else 
    {
        digitalWrite(ENABLE_PIN, HIGH);  // ปิดมอเตอร์ถ้าไม่กดปุ่ม
    }
}

  void stepMotor(int stepPin, int delayTime) 
  {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(delayTime);  
      digitalWrite(stepPin, LOW);
      delayMicroseconds(delayTime);  
  }

  void forwardDirection() 
  {
    digitalWrite(DIR_PIN_1, HIGH);  
    digitalWrite(DIR_PIN_2, LOW);   
    digitalWrite(DIR_PIN_3, HIGH);  
    digitalWrite(DIR_PIN_4, LOW);   
  }

  void reverseDirection() 
  {
    digitalWrite(DIR_PIN_1, LOW);   
    digitalWrite(DIR_PIN_2, HIGH);  
    digitalWrite(DIR_PIN_3, LOW);   
    digitalWrite(DIR_PIN_4, HIGH);  
  }

  void turnLeft() 
  {
    digitalWrite(DIR_PIN_2, HIGH);
    digitalWrite(DIR_PIN_4, HIGH);
  }

  void turnRight() 
  {
    digitalWrite(DIR_PIN_1, LOW);
    digitalWrite(DIR_PIN_3, LOW);
  }
  void moveUp()
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }

  void moveDown() 
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }

  void stopMotor() 
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }