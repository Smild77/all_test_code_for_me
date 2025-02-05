#include <Arduino.h>
// Motor 1
#define DIR_PIN_1 4
#define STEP_PIN_1 5
#define ENABLE_PIN_1 6
// Motor 2
#define DIR_PIN_2 7
#define STEP_PIN_2 9
#define ENABLE_PIN_2 8
// Switch
#define SWITCH_START_1 A2
#define SWITCH_START_2 A0
#define SWITCH_REVERSE_1 A3
#define SWITCH_REVERSE_2 A1

bool isRunning[2] = {false, false};
bool direction[2] = {true, true};
unsigned long speedDelay[2] = {260, 260};
unsigned long lastPulseTime[2] = {0, 0};
bool stepState[2] = {false, false};
int pwmValue[2] = {0, 0};
String lastCommand = "";

void stopAllMotors();
void handleSerialCommand();
void handleButtonPress();
void controlMotor(int motorIndex, bool state, bool dir);
void controlMotorsSequential(int motor1State, int motor2State, bool motor1Dir, bool motor2Dir)
{
  controlMotor(0, motor1State, motor1Dir);
  delay(1000);
  controlMotor(1, motor2State, motor2Dir);
}

void setup()
{
  pinMode(DIR_PIN_1, OUTPUT);
  pinMode(STEP_PIN_1, OUTPUT);
  pinMode(ENABLE_PIN_1, OUTPUT);
  pinMode(DIR_PIN_2, OUTPUT);
  pinMode(STEP_PIN_2, OUTPUT);
  pinMode(ENABLE_PIN_2, OUTPUT);

  pinMode(SWITCH_START_1, INPUT_PULLUP);
  pinMode(SWITCH_START_2, INPUT_PULLUP);
  pinMode(SWITCH_REVERSE_1, INPUT_PULLUP);
  pinMode(SWITCH_REVERSE_2, INPUT_PULLUP);

  digitalWrite(ENABLE_PIN_1, HIGH);
  digitalWrite(ENABLE_PIN_2, HIGH);

  Serial.begin(9600);
  Serial.println("Setup complete");
}

void loop()
{
  handleButtonPress();
  handleSerialCommand();

  if (isRunning[0])
  {
    if (micros() - lastPulseTime[0] >= speedDelay[0])
    {
      lastPulseTime[0] = micros();
      digitalWrite(STEP_PIN_1, HIGH);
      delayMicroseconds(2);
      digitalWrite(STEP_PIN_1, LOW);
    }
  }
  else
  {
    digitalWrite(STEP_PIN_1, LOW);
  }

  if (isRunning[1])
  {
    if (micros() - lastPulseTime[1] >= speedDelay[1])
    {
      lastPulseTime[1] = micros();
      digitalWrite(STEP_PIN_2, HIGH);
      delayMicroseconds(2);
      digitalWrite(STEP_PIN_2, LOW);
    }
  }
  else
  {
    digitalWrite(STEP_PIN_2, LOW);
  }
}

void handleButtonPress()
{
  static unsigned long lastDebounceTimeStart[2] = {0, 0};
  static unsigned long lastDebounceTimeReverse[2] = {0, 0};
  const unsigned long debounceDelay = 50;
  static int lastSwitchStateStart[2] = {HIGH, HIGH};
  static int lastSwitchStateReverse[2] = {HIGH, HIGH};
  static int buttonStateStart[2] = {HIGH, HIGH};
  static int buttonStateReverse[2] = {HIGH, HIGH};

  for (int i = 0; i < 2; i++)
  {
    int switchPin = (i == 0) ? SWITCH_START_1 : SWITCH_START_2;
    int currentSwitchState = digitalRead(switchPin);

    if (currentSwitchState != lastSwitchStateStart[i])
    {
      lastDebounceTimeStart[i] = millis();
    }

    if ((millis() - lastDebounceTimeStart[i]) > debounceDelay)
    {
      if (currentSwitchState != buttonStateStart[i])
      {
        buttonStateStart[i] = currentSwitchState;

        if (buttonStateStart[i] == LOW)
        {
          if (!isRunning[i])
          {
            isRunning[i] = true;

            if (i == 0)
            {
              digitalWrite(ENABLE_PIN_1, LOW);
              digitalWrite(DIR_PIN_1, direction[i] ? HIGH : LOW);
            }
            else
            {
              digitalWrite(ENABLE_PIN_2, LOW);
              digitalWrite(DIR_PIN_2, direction[i] ? HIGH : LOW);
            }

            Serial.print("Motor ");
            Serial.print(i + 1);
            Serial.println(" started");
          }
        }
        else
        {
          if (isRunning[i])
          {
            isRunning[i] = false;

            if (i == 0)
            {
              digitalWrite(ENABLE_PIN_1, HIGH);
            }
            else
            {
              digitalWrite(ENABLE_PIN_2, HIGH);
            }

            Serial.print("Motor ");
            Serial.print(i + 1);
            Serial.println(" stopped");
          }
        }
      }
    }
    lastSwitchStateStart[i] = currentSwitchState;
  }

  for (int i = 0; i < 2; i++)
  {
    int switchReversePin = (i == 0) ? SWITCH_REVERSE_1 : SWITCH_REVERSE_2;
    int currentSwitchState = digitalRead(switchReversePin);

    if (currentSwitchState != lastSwitchStateReverse[i])
    {
      lastDebounceTimeReverse[i] = millis();
    }

    if ((millis() - lastDebounceTimeReverse[i]) > debounceDelay)
    {
      if (currentSwitchState != buttonStateReverse[i])
      {
        buttonStateReverse[i] = currentSwitchState;

        if (buttonStateReverse[i] == LOW)
        {
          if (!isRunning[i])
          {
            isRunning[i] = true;

            if (i == 0)
            {
              digitalWrite(ENABLE_PIN_1, LOW);
              digitalWrite(DIR_PIN_1, direction[i] ? LOW : HIGH);
            }
            else
            {
              digitalWrite(ENABLE_PIN_2, LOW);
              digitalWrite(DIR_PIN_2, direction[i] ? LOW : HIGH);
            }

            Serial.print("Motor ");
            Serial.print(i + 1);
            Serial.println(" started (reverse)");
          }
        }
        else
        {
          if (isRunning[i])
          {
            isRunning[i] = false;

            if (i == 0)
            {
              digitalWrite(ENABLE_PIN_1, HIGH);
            }
            else
            {
              digitalWrite(ENABLE_PIN_2, HIGH);
            }

            Serial.print("Motor ");
            Serial.print(i + 1);
            Serial.println(" stopped");
          }
        }
      }
    }
    lastSwitchStateReverse[i] = currentSwitchState;
  }
}

void controlMotor(int motorIndex, bool state, bool dir)
{
  isRunning[motorIndex] = state;

  if (motorIndex == 0)
  {
    digitalWrite(ENABLE_PIN_1, !state);
    digitalWrite(DIR_PIN_1, dir ? HIGH : LOW);
  }
  else
  {
    digitalWrite(ENABLE_PIN_2, !state);
    digitalWrite(DIR_PIN_2, dir ? HIGH : LOW);
  }

  Serial.print("Motor ");
  Serial.print(motorIndex + 1);
  Serial.println(state ? " started" : " stopped");
}

void handleSerialCommand()
{
  if (Serial.available())
  {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "stop all")
    {
      stopAllMotors();
      return;
    }

    command.toLowerCase();
    if (command.length() > 5 || command == lastCommand)
      return;
    lastCommand = command;

    if (command.startsWith("p1") || command.startsWith("p2"))
    {
      int motorIndex = (command.charAt(1) == '1') ? 0 : 1;
      int speedLevel = command.substring(2).toInt();

      Serial.print("Received command: ");
      Serial.println(command);
      Serial.print("Extracted motor index: ");
      Serial.println(motorIndex);
      Serial.print("Extracted Speed Level: ");
      Serial.println(speedLevel);

      if (speedLevel > 100)
      {
        Serial.println("❌ Invalid Speed Level (ต้องอยู่ระหว่าง 0-100%)");
        return;
      }

      speedDelay[motorIndex] = map(speedLevel, 0, 100, 2000, 260);
      Serial.print("✅ Motor ");
      Serial.print(motorIndex + 1);
      Serial.print(" Speed set to ");
      Serial.print(speedLevel);
      Serial.print("% (Mapped delay: ");
      Serial.print(speedDelay[motorIndex]);
      Serial.println(" us)");

      if (!isRunning[motorIndex])
      {
        controlMotor(motorIndex, true, direction[motorIndex]);
      }
    }
    else if (command == "m1")
    {
      controlMotor(0, true, true);
      speedDelay[0] = 260;
      Serial.println("Motor 1 started at default speed (260us delay)");
    }
    else if (command == "m1r")
    {
      controlMotor(0, true, false);
      speedDelay[0] = 260;
    }
    else if (command == "m1s")
    {
      controlMotor(0, false, true);
    }
    else if (command == "m2")
    {
      controlMotor(1, true, true);
      speedDelay[1] = 260;
      Serial.println("Motor 2 started at default speed (260us delay)");
    }
    else if (command == "m2r")
    {
      controlMotor(1, true, false);
      speedDelay[1] = 260;
    }
    else if (command == "m2s")
    {
      controlMotor(1, false, true);
    }
    else if (command == "m1m2")
    {
      controlMotorsSequential(true, true, true, true);
      speedDelay[0] = 260;
    }
    else if (command == "m1m2r")
    {
      controlMotorsSequential(true, true, false, false);
      Serial.println("✅ Both motors started in reverse");
    }
    else if (command == "m1m2s")
    {
      controlMotorsSequential(false, false, true, true);
    }
    else
    {
      Serial.println("❌ Invalid command");
    }
  }
}

void stopAllMotors()
{
  isRunning[0] = false;
  isRunning[1] = false;

  digitalWrite(ENABLE_PIN_1, HIGH);
  digitalWrite(ENABLE_PIN_2, HIGH);

  digitalWrite(STEP_PIN_1, LOW);
  digitalWrite(STEP_PIN_2, LOW);

  Serial.println("✅ All motors stopped");
}
