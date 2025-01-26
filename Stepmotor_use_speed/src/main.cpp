#include <Arduino.h>
#include <SoftwareSerial.h>

// กำหนด SoftwareSerial
#define RX_PIN 10                        // รับข้อมูลจาก Master
#define TX_PIN 11                        // ส่งข้อมูลกลับไปยัง Master
SoftwareSerial mySerial(RX_PIN, TX_PIN); // RX, TX

// มอเตอร์ 1
#define DIR_PIN_1 4
#define STEP_PIN_1 5
#define ENABLE_PIN_1 6

// มอเตอร์ 2
#define DIR_PIN_2 7
#define STEP_PIN_2 8
#define ENABLE_PIN_2 9

// สวิตช์
#define SWITCH_START_1 A2
#define SWITCH_START_2 A0
#define SWITCH_REVERSE_1 A3
#define SWITCH_REVERSE_2 A1

bool isRunning[2] = {false, false};       // สถานะมอเตอร์
bool direction[2] = {true, true};         // ทิศทางมอเตอร์
unsigned long speedDelay[2] = {100, 100}; // ความเร็วมอเตอร์
unsigned long lastPulseTime[2] = {0, 0};  // เวลา Pulse ล่าสุด
bool stepState[2] = {false, false};       // สถานะ STEP_PIN
bool buttonPressed[2] = {false, false};   // สถานะปุ่มกด
bool serialCommand[2] = {false, false};   // สถานะ Serial Command
String lastCommand = "";                  // เก็บคำสั่งล่าสุด

void handleButtonPress();
void handleSerialCommand();

void setup()
{
  for (int i = 0; i < 2; i++)
  {
    pinMode(DIR_PIN_1 + (i * 3), OUTPUT);
    pinMode(STEP_PIN_1 + (i * 3), OUTPUT);
    pinMode(ENABLE_PIN_1 + (i * 3), OUTPUT);
    pinMode((i == 0 ? SWITCH_START_2 : SWITCH_START_1), INPUT_PULLUP);
    pinMode((i == 0 ? SWITCH_REVERSE_2 : SWITCH_REVERSE_1), INPUT_PULLUP);
    digitalWrite(ENABLE_PIN_1 + (i * 3), HIGH);
  }

  Serial.begin(9600);   // สำหรับ Debug
  mySerial.begin(9600); // SoftwareSerial สำหรับสื่อสารกับ Master
  Serial.println("Setup complete");
}

void loop()
{
  handleButtonPress();

  for (int i = 0; i < 2; i++)
  {
    if (isRunning[i])
    {
      if (micros() - lastPulseTime[i] >= speedDelay[i])
      {
        lastPulseTime[i] = micros();
        stepState[i] = !stepState[i];
        digitalWrite(STEP_PIN_1 + (i * 3), stepState[i]);
      }
    }
  }

  handleSerialCommand(); // แยกการจัดการคำสั่ง Serial ออกจาก loop
}
void handleButtonPress()
{
  for (int i = 0; i < 2; i++)
  {
    int startSwitchState = digitalRead((i == 0 ? SWITCH_START_2 : SWITCH_START_1));
    int reverseSwitchState = digitalRead((i == 0 ? SWITCH_REVERSE_2 : SWITCH_REVERSE_1));

    // กดค้างเพื่อหมุนตามทิศทางปัจจุบัน
    if (startSwitchState == LOW)
    {
      if (!isRunning[i])
      {
        isRunning[i] = true;
        digitalWrite(ENABLE_PIN_1 + (i * 3), LOW); // เปิดไดรเวอร์มอเตอร์
        digitalWrite(DIR_PIN_1 + (i * 3), direction[i] ? HIGH : LOW);
        Serial.print("Motor ");
        Serial.print(i + 1);
        Serial.println(" started via Hold Button (current direction)");
      }
    }
    else
    {
      if (isRunning[i])
      {
        isRunning[i] = false;
        digitalWrite(ENABLE_PIN_1 + (i * 3), HIGH); // ปิดไดรเวอร์มอเตอร์
        Serial.print("Motor ");
        Serial.print(i + 1);
        Serial.println(" stopped via Release Button");
      }
    }

    // กดค้างเพื่อหมุนในทิศทางกลับด้าน
    if (reverseSwitchState == LOW)
    {
      if (!isRunning[i])
      {
        isRunning[i] = true;
        digitalWrite(ENABLE_PIN_1 + (i * 3), LOW); // เปิดไดรเวอร์มอเตอร์
        digitalWrite(DIR_PIN_1 + (i * 3), !direction[i] ? HIGH : LOW);
        Serial.print("Motor ");
        Serial.print(i + 1);
        Serial.println(" started via Hold Reverse Button (reverse direction)");
      }
    }
    else
    {
      if (isRunning[i])
      {
        isRunning[i] = false;
        digitalWrite(ENABLE_PIN_1 + (i * 3), HIGH); // ปิดไดรเวอร์มอเตอร์
        Serial.print("Motor ");
        Serial.print(i + 1);
        Serial.println(" stopped via Release Reverse Button");
      }
    }
  }
}
void handleSerialCommand()
{
  if (mySerial.available())
  {
    String command = mySerial.readStringUntil('\n');
    command.trim();

    if (command.length() == 0)
    {
      Serial.println("Received empty command. Ignored.");
      return;
    }

    command.toLowerCase();

    if (command.length() > 4)
    {
      Serial.println("Invalid command length: " + command);
      mySerial.println("Invalid command length");
      return;
    }

    if (command == lastCommand && command != "m1s" && command != "m2s")
    {
      Serial.println("Duplicate command ignored: " + command);
      return;
    }
    lastCommand = command;

    Serial.print("Received command: ");
    Serial.println(command);

    // ตรวจสอบคำสั่ง
    if (command.startsWith("m1v") || command.startsWith("m2v"))
    {
      char motor = command[1];
      char speed = command[3];

      // เลือกมอเตอร์ที่ถูกต้อง
      int motorIndex = (motor == '1') ? 0 : (motor == '2') ? 1
                                                           : -1;
      if (motorIndex == -1)
      {
        Serial.println("Invalid motor selection");
        mySerial.println("Invalid motor selection");
        return;
      }

      // เลือกระดับความเร็ว
      switch (speed)
      {
      case '0':
        speedDelay[motorIndex] = 200; // ความเร็วช้า
        Serial.print("Motor ");
        Serial.print(motor);
        Serial.println(" set to lowest speed");
        mySerial.println("Motor set to lowest speed");
        break;
      case '1':
        speedDelay[motorIndex] = 150; // ความเร็วระดับ 1
        Serial.print("Motor ");
        Serial.print(motor);
        Serial.println(" set to speed level 1");
        mySerial.println("Motor set to speed level 1");
        break;
      case '2':
        speedDelay[motorIndex] = 100; // ความเร็วระดับ 2
        Serial.print("Motor ");
        Serial.print(motor);
        Serial.println(" set to speed level 2");
        mySerial.println("Motor set to speed level 2");
        break;
      case '3':
        speedDelay[motorIndex] = 70; // ความเร็วระดับ 3
        Serial.print("Motor ");
        Serial.print(motor);
        Serial.println(" set to speed level 3");
        mySerial.println("Motor set to speed level 3");
        break;
      case '4':
        speedDelay[motorIndex] = 40; // ความเร็วระดับ 4
        Serial.print("Motor ");
        Serial.print(motor);
        Serial.println(" set to speed level 4");
        mySerial.println("Motor set to speed level 4");
        break;
      case '5':
        speedDelay[motorIndex] = 13; // ความเร็วสูงสุด
        Serial.print("Motor ");
        Serial.print(motor);
        Serial.println(" set to maximum speed");
        mySerial.println("Motor set to maximum speed");
        break;
      default:
        Serial.println("Invalid speed level");
        mySerial.println("Invalid speed level");
        break;
      }
    }
    else if (command == "m1")
    {
      serialCommand[0] = true;
      buttonPressed[0] = false;
      isRunning[0] = true;
      digitalWrite(ENABLE_PIN_1, LOW);
      digitalWrite(DIR_PIN_1, HIGH);
      Serial.println("Motor 1 started clockwise via Serial Command");
      mySerial.println("Motor 1 started clockwise");
    }
    else if (command == "m1r")
    {
      serialCommand[0] = true;
      buttonPressed[0] = false;
      isRunning[0] = true;
      digitalWrite(ENABLE_PIN_1, LOW);
      digitalWrite(DIR_PIN_1, LOW);
      Serial.println("Motor 1 started counterclockwise via Serial Command");
      mySerial.println("Motor 1 started counterclockwise");
    }
    else if (command == "m1s")
    {
      serialCommand[0] = false;
      isRunning[0] = false;
      digitalWrite(ENABLE_PIN_1, HIGH);
      Serial.println("Motor 1 stopped via Serial Command");
      mySerial.println("Motor 1 stopped");
    }
    else if (command == "m2")
    {
      serialCommand[1] = true;
      buttonPressed[1] = false;
      isRunning[1] = true;
      digitalWrite(ENABLE_PIN_2, LOW);
      digitalWrite(DIR_PIN_2, HIGH);
      Serial.println("Motor 2 started clockwise via Serial Command");
      mySerial.println("Motor 2 started clockwise");
    }
    else if (command == "m2r")
    {
      serialCommand[1] = true;
      buttonPressed[1] = false;
      isRunning[1] = true;
      digitalWrite(ENABLE_PIN_2, LOW);
      digitalWrite(DIR_PIN_2, LOW);
      Serial.println("Motor 2 started counterclockwise via Serial Command");
      mySerial.println("Motor 2 started counterclockwise");
    }
    else if (command == "m2s")
    {
      serialCommand[1] = false;
      isRunning[1] = false;
      digitalWrite(ENABLE_PIN_2, HIGH);
      Serial.println("Motor 2 stopped via Serial Command");
      mySerial.println("Motor 2 stopped");
    }
    else if (command == "help")
    {
      Serial.println("Available commands:");
      Serial.println("  m1    : Start Motor 1 clockwise");
      Serial.println("  m1r   : Start Motor 1 counterclockwise");
      Serial.println("  m1v   : Stop Motor 1");
      Serial.println("  m1vX  : Set Motor 1 speed (X = 0-5)");
      Serial.println("  m2    : Start Motor 2 clockwise");
      Serial.println("  m2r   : Start Motor 2 counterclockwise");
      Serial.println("  m2s   : Stop Motor 2");
      Serial.println("  m2vX  : Set Motor 2 speed (X = 0-5)");
      Serial.println("  help  : Show this help message");
      mySerial.println("Help displayed");
    }
    else
    {
      Serial.println("Invalid command received: " + command);
      mySerial.println("Invalid command received");
    }
  }
}
