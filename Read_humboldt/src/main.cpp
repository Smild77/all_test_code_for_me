#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <SoftwareSerial.h>

// กำหนด ADS1115
Adafruit_ADS1115 ads1(0x48); // ตัวที่ 1 (ADDR → GND)
Adafruit_ADS1115 ads2(0x49); // ตัวที่ 2 (ADDR → VDD)

// กำหนด SoftwareSerial
#define RX_PIN 10                        // รับข้อมูลจาก Slave
#define TX_PIN 11                        // ส่งข้อมูลไปยัง Slave
SoftwareSerial mySerial(RX_PIN, TX_PIN); // RX, TX

// ค่าคงที่สำหรับการคำนวณระยะทาง
float SCALE_FACTOR1 = 885.42; // mm/V สำหรับ Sensor 1
float SCALE_FACTOR2 = 1313.56; // mm/V สำหรับ Sensor 2
float baseVoltage1 = 3.2272;
float baseVoltage2 = 3.2268;

bool isReadingSensors = true; // สถานะการอ่านเซ็นเซอร์

// ฟังก์ชันต้นแบบ
float readAverageVoltage(Adafruit_ADS1115 &ads, int samples);
void readAndDisplaySensors();
void handleSerialCommands();
void printHelp();

void setup()
{
  Serial.begin(9600);   // ใช้สำหรับ Debug
  mySerial.begin(9600); // ใช้สำหรับสื่อสารกับ Slave

  Serial.println("System Initialized");
  Wire.begin();
  ads1.begin();
  ads2.begin();

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

  // อ่านข้อมูลจาก Slave ผ่าน SoftwareSerial
  if (mySerial.available())
  {
    String response = mySerial.readStringUntil('\n');
    Serial.print("Slave Response: ");
    Serial.println(response);
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

    Serial.print("Sensor 1 -> Voltage: ");
    Serial.print(voltage1, 4);
    Serial.print(" V | Distance: ");
    Serial.print(distance1, 2);
    Serial.println(" mm");

    Serial.print("Sensor 2 -> Voltage: ");
    Serial.print(voltage2, 4);
    Serial.print(" V | Distance: ");
    Serial.print(distance2, 2);
    Serial.println(" mm");
  }
}

// ฟังก์ชันอ่านค่าแรงดันเฉลี่ย
float readAverageVoltage(Adafruit_ADS1115 &ads, int samples)
{
  float totalVoltage = 0;
  for (int i = 0; i < samples; i++)
  {
    int16_t adcValue = ads.readADC_SingleEnded(0);
    totalVoltage += adcValue * 0.1875 / 1000; // แปลง ADC เป็นแรงดันไฟฟ้า
    delay(10);
  }
  return totalVoltage / samples;
}

// ฟังก์ชันจัดการคำสั่ง Serial
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
      isReadingSensors = false; // หยุดการอ่านเซ็นเซอร์
      Serial.println("Sensor reading stopped.");
    }
    else if (command == "startsensors")
    {
      isReadingSensors = true; // เริ่มการอ่านเซ็นเซอร์ใหม่
      Serial.println("Sensor reading started.");
    }
    else if (command == "d1")
    {
      // ส่งคำสั่ง 'd1' ไปยัง Slave
      mySerial.println("d1");
      Serial.println("Command 'd1' sent to Slave.");

      // รอการตอบกลับจาก Slave
      unsigned long startTime = millis();
      bool responseReceived = false;

      while (millis() - startTime < 1000) // รอ 1 วินาที
      {
        if (mySerial.available())
        {
          String response = mySerial.readStringUntil('\n');
          Serial.print("Slave Response: ");
          Serial.println(response);

          if (response.length() > 0)
          {
            responseReceived = true;
            break;
          }
        }
      }

      if (responseReceived)
      {
        // ถ้าได้รับการตอบกลับ ให้ส่ง 'idl'
        mySerial.println("idl");
        Serial.println("Command 'idl' sent to Slave.");
      }
      else
      {
        Serial.println("No response from Slave.");
      }
    }
    else if (command.startsWith("m1"))
    {
      mySerial.println(command); // ส่งคำสั่งไปยัง Slave ผ่าน SoftwareSerial
      Serial.print("Command sent to Slave: ");
      Serial.println(command);
    }
    else if (command.startsWith("m2"))
    {
      mySerial.println(command); // ส่งคำสั่งไปยัง Slave ผ่าน SoftwareSerial
      Serial.print("Command sent to Slave: ");
      Serial.println(command);
    }
    else if (command.startsWith("s1"))
    {
      mySerial.println(command); // ส่งคำสั่งไปยัง Slave ผ่าน SoftwareSerial
      Serial.print("Speed command sent to Slave: ");
      Serial.println(command);
    }
    else if (command.startsWith("s1"))
    {
      mySerial.println(command); // ส่งคำสั่งไปยัง Slave ผ่าน SoftwareSerial
      Serial.print("Speed command sent to Slave: ");
      Serial.println(command);
    }
    else if (command == "STOP_ALL")
    {
      mySerial.println(command); // ส่งคำสั่งหยุดทั้งหมดไปยัง Slave
      Serial.println("Stop all command sent to Slave.");
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
  Serial.println("help       - Display this help message");
  Serial.println("m1   - Start Motor 1 clockwise");
  Serial.println("m1r - Start Motor 1 counterclockwise");
  Serial.println("m1s    - Stop Motor 1");
  Serial.println("m2   - Start Motor 2 clockwise");
  Serial.println("m2r - Start Motor 2 counterclockwise");
  Serial.println("m2s    - Stop Motor 2");
  Serial.println("s1_<1-5> - Set speed for Motor 1 (1=slow, 5=fast)");
  Serial.println("s2_<1-5> - Set speed for Motor 2 (1=slow, 5=fast)");
  Serial.println("STOP_ALL   - Stop all motors");
  Serial.println("STOP_READING - Stop distance measurement");
  Serial.println("START_READING - Start distance measurement");
  Serial.println("d1         - Send 'd1' to Slave and wait for response. Send 'idl' if response is received.");
}
