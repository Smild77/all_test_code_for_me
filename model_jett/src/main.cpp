#include <Arduino.h>
#define RXD2 16
#define TXD2 17

const int stepPin = 11;    // Pin for stepper motor step
const int dirPin = 7;     // Pin for stepper motor direction
const int enb_pin = 15;    // Pin for stepper motor enable
const int proximity_sensorA = 46; // Pin for proximity sensor
const int proximity_sensorB = 47; // Pin for proximity sensor

bool isRunning = false;
unsigned long lastPulseTime = 0;
unsigned long pulseInterval = 100; // Default to lowest speed 150 

bool stepState = false;
bool debug_to_a = false;
bool debug_to_b = false;

bool lock_work_m_move = false;

void serialEvent_D(void);
void serialEvent_B(void);

void setup() 
{
  Serial.begin(115200);
  Serial2.begin(115200,SERIAL_8N1);
  pinMode(proximity_sensorA, INPUT_PULLUP);
  pinMode(proximity_sensorB, INPUT_PULLUP);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enb_pin, OUTPUT);
  digitalWrite(enb_pin,HIGH);
}

void loop() 
{
  if (isRunning && (micros() - lastPulseTime >= pulseInterval))
  {
    lastPulseTime = micros();
    stepState = !stepState;
    digitalWrite(stepPin, stepState);
  }
  // if (Serial.available() > 0) 
  if (Serial2.available() > 0)
  {
    // String command = Serial.readStringUntil('\n');
    String command = Serial2.readStringUntil('\n');
    command.trim();
    
    if (command == "A") 
    {
      isRunning = true;
      lock_work_m_move = true;
      digitalWrite(enb_pin, LOW);
      digitalWrite(dirPin, HIGH);
    } 
    else if (command == "B") 
    {
      isRunning = true;
      lock_work_m_move = true;
      digitalWrite(enb_pin, LOW);
      digitalWrite(dirPin, LOW);
    } 
    else if (command == "da")
    {
      debug_to_a = true;
      debug_to_b = false;
    }
    else if (command == "db")
    {
      debug_to_b = true;
      debug_to_a = false;
    }
    else if (command == "s")
    {
      isRunning = false;
      digitalWrite(enb_pin, HIGH);
    }
    else if (command == "d")
    {
      serialEvent_D();
    }
    else if (command == "b")
    {
      serialEvent_B();
    }
    else 
    {
      // Serial.println("Unknown command");
    }
  }
  //============================================= DEBGU MODE ==============================================

  int sensorValue1 = digitalRead(proximity_sensorA);
  int sensorValue2 = digitalRead(proximity_sensorB);
  if(lock_work_m_move == true)
  {
    if ((sensorValue1 == LOW && digitalRead(dirPin) == HIGH)) 
    // if ((debug_to_a == true && digitalRead(dirPin) == HIGH) || (debug_to_b == true && digitalRead(dirPin) == LOW)) // is debug mode
    {
      // Serial.println("AOK");
      isRunning = false;
      digitalWrite(enb_pin, HIGH);
      delay(300);
      Serial2.println("AOK");
      lock_work_m_move = false;
    }
  else if ((sensorValue2 == LOW && digitalRead(dirPin) == LOW))
    {
      // Serial.println("BOK");
      isRunning = false;
      digitalWrite(enb_pin, HIGH);
      delay(300);
      Serial2.println("BOK");
      lock_work_m_move = false;
    }
  }
  //============================================= DEBGU MODE ==============================================
}

void serialEvent_D()
{
  delay(1000);
  Serial2.println("DOK");
}

void serialEvent_B()
{
  delay(1000);
  Serial2.println("BOK");
}