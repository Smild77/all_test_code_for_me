#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>

// กำหนดที่อยู่ของเซ็นเซอร์
Adafruit_ADS1115 ads1(0x48); // ตัวที่ 1 (ADDR → GND)
Adafruit_ADS1115 ads2(0x49); // ตัวที่ 2 (ADDR → VDD)

// ค่าคงที่สำหรับการคำนวณระยะทาง
float SCALE_FACTOR1 = 625.990; // mm/V สำหรับ Sensor 1
float SCALE_FACTOR2 = 925.864; // mm/V สำหรับ Sensor 2
float baseVoltage1 = 3.54;
float baseVoltage2 = 3.54;

bool isReadingSensors = true;

// ฟังก์ชัน
float readAverageVoltage(Adafruit_ADS1115 &ads, int samples);
void readAndDisplaySensors();
void handleSerialCommands();
void printHelp();

void setup()
{
  Serial.begin(9600);
  Serial.println("System Initialized");
  Wire.begin();
  ads1.begin();
  ads2.begin();

  // ตั้งค่า Gain หลังจากเริ่มต้น ADS1115
  ads1.setGain(GAIN_ONE);
  ads2.setGain(GAIN_ONE);

  baseVoltage1 = readAverageVoltage(ads1, 20);
  baseVoltage2 = readAverageVoltage(ads2, 20);
}

void loop()
{
  handleSerialCommands(); // ตรวจสอบคำสั่งจาก Serial Monitor
  if (isReadingSensors)
  {
    readAndDisplaySensors(); // อ่านค่าเซ็นเซอร์และแสดงผล
  }
}

void readAndDisplaySensors()
{
  static unsigned long lastSensorReadTime = 0;
  const unsigned long sensorReadInterval = 500; // 500 ms

  if (millis() - lastSensorReadTime >= sensorReadInterval)
  {
    lastSensorReadTime = millis();

    float voltage1 = readAverageVoltage(ads1, 20);
    float voltage2 = readAverageVoltage(ads2, 20);
    float distance1 = max(0.0, SCALE_FACTOR1 * (voltage1 - baseVoltage1));
    float distance2 = max(0.0, SCALE_FACTOR2 * (voltage2 - baseVoltage2));

    Serial.print(distance1, 3);
    Serial.print(",");
    Serial.println(distance2, 3);
  }
}

float readAverageVoltage(Adafruit_ADS1115 &ads, int samples)
{
  double totalVoltage = 0;
  for (int i = 0; i < samples; i++)
  {
    int16_t adcValue = ads.readADC_SingleEnded(0);
    totalVoltage += adcValue * 0.1875 / 1000.0;
  }
  return totalVoltage / samples;
}

// ฟังก์ชันตรวจสอบคำสั่งจาก Serial Monitor
void handleSerialCommands()
{
  if (Serial.available())
  {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "help")
    {
      printHelp();
    }
    else if (command == "stopsensors")
    {
      isReadingSensors = false;
      Serial.println("Sensor reading stopped.");
    }
    else if (command == "startsensors")
    {
      isReadingSensors = true;
      Serial.println("Sensor reading started.");
    }
    else if (command == "STOP_READING")
    {
      isReadingSensors = false;
      Serial.println("Distance measurement stopped.");
    }
    else if (command == "START_READING")
    {
      isReadingSensors = true;
      Serial.println("Distance measurement started.");
    }
    else
    {
      Serial.println("Invalid command. Type 'help' to see the list of available commands.");
    }
  }
}

void printHelp()
{
  Serial.println("Available commands:");
  Serial.println("help          - Display this help message");
  Serial.println("stopsensors   - Stop sensor readings");
  Serial.println("startsensors  - Start sensor readings");
  Serial.println("STOP_READING  - Stop distance measurement");
  Serial.println("START_READING - Start distance measurement");
}
