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

bool isRunning = false;
bool direction = true;
unsigned long speedDelay = 260;
unsigned long lastPulseTime = 0;
bool stepState = false;
String lastCommand = "";

void stopAllMotors();
void handleSerialCommand();
void handleButtonPress();
void controlMotor(int motorIndex, bool state, bool dir);

void setup()
{
    pinMode(DIR_PIN_2, OUTPUT);
    pinMode(STEP_PIN_2, OUTPUT);
    pinMode(ENABLE_PIN_2, OUTPUT);
    pinMode(SWITCH_START_2, INPUT_PULLUP);
    pinMode(SWITCH_REVERSE_2, INPUT_PULLUP);
    digitalWrite(ENABLE_PIN_2, HIGH);

    Serial.begin(9600);
    Serial.println("Setup complete");
}

void loop()
{
    handleSerialCommand();
    handleButtonPress();
    if (isRunning)
    {
      if (micros() - lastPulseTime >= speedDelay)
      {
        lastPulseTime = micros();
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
          if (!isRunning)
          {
            isRunning = true;

            if (i == 0)
            {
              digitalWrite(ENABLE_PIN_1, LOW);
              digitalWrite(DIR_PIN_1, direction ? HIGH : LOW);
            }
            else
            {
              digitalWrite(ENABLE_PIN_2, LOW);
              digitalWrite(DIR_PIN_2, direction ? HIGH : LOW);
            }

            Serial.print("Motor ");
            Serial.print(i + 1);
            Serial.println(" started");
          }
        }
        else
        {
          if (isRunning)
          {
            isRunning = false;

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
          if (!isRunning)
          {
            isRunning = true;

            if (i == 0)
            {
              digitalWrite(ENABLE_PIN_1, LOW);
              digitalWrite(DIR_PIN_1, direction ? LOW : HIGH);
            }
            else
            {
              digitalWrite(ENABLE_PIN_2, LOW);
              digitalWrite(DIR_PIN_2, direction ? LOW : HIGH);
            }

            Serial.print("Motor ");
            Serial.print(i + 1);
            Serial.println(" started (reverse)");
          }
        }
        else
        {
          if (isRunning)
          {
            isRunning = false;

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
    isRunning = state;

    digitalWrite(ENABLE_PIN_2, !state);
    digitalWrite(DIR_PIN_2, dir ? HIGH : LOW);

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

        if (command.startsWith("p2"))
        {
            int speedLevel = command.substring(2).toInt();

            Serial.print("Received command: ");
            Serial.println(command);
            Serial.print("Extracted Speed Level: ");
            Serial.println(speedLevel);

            if (speedLevel < 0 || speedLevel > 100)
            {
                Serial.println("❌ Invalid Speed Level (ต้องอยู่ระหว่าง 0-100%)");
                return;
            }

            speedDelay = map(speedLevel, 0, 100, 100000, 260);  

            Serial.print("✅ Motor Speed set to ");
            Serial.print(speedLevel);
            Serial.print("% (Mapped delay: ");
            Serial.print(speedDelay);
            Serial.println(" us)");

         if (!isRunning)
        {
          controlMotor(0, true, direction);  
        }
        }
        else if (command == "m2")
        {
            controlMotor(0, true, true);  
        }
        else if (command == "m2r")
        {
            controlMotor(0, true, false);  
        }
        else if (command == "m2s")
        {
            controlMotor(0, false, true);  
        }
        else
        {
            Serial.println("❌ Invalid command");
        }
    }
}

void stopAllMotors()
{
    isRunning = false;
    digitalWrite(ENABLE_PIN_2, HIGH);
    digitalWrite(STEP_PIN_2, LOW);
    Serial.println("✅ All motors stopped");
}
