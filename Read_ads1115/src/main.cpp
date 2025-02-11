#include <Arduino.h>
#include <ADS1X15.h>

ADS1115 ADS1(0x48);  // ที่อยู่ I2C ของเซ็นเซอร์ตัวที่ 1
ADS1115 ADS2(0x49);  // ที่อยู่ I2C ของเซ็นเซอร์ตัวที่ 2

float displacement1 = 0.0;
float displacement2 = 0.0;
float average1 = 0.0;
float average2 = 0.0;
static float sum1 = 0.0;  
static float sum2 = 0.0;  
static int count = 0;    
static float displacementList1[250] = {0};
static float displacementList2[250] = {0};

void setup() 
{
  Serial.begin(115200);
  Serial.print("System online 🌟 ✅\n");
  Wire.begin();
  ADS1.begin();
  ADS1.setGain(16);
  ADS1.setDataRate(7);
  ADS2.begin();
  ADS2.setGain(16);
  ADS2.setDataRate(7);
}

void loop() 
{
    int16_t val_01 = ADS1.readADC_Differential_0_1();  
    int16_t val_02 = ADS2.readADC_Differential_0_1();
    float volts_01 = ADS1.toVoltage(val_01); 
    float volts_02 = ADS2.toVoltage(val_02); 

    displacement1 = 100*((volts_01 / 3.65) * 17.00); 
    displacement2 = 100*((volts_02 / 3.65) * 25.00);

    displacementList1[count] = displacement1;
    sum1 += displacement1;
    displacementList2[count] = displacement2;
    sum2 += displacement2;
    count++;

    if (count == 250) {
      average1 = sum1 / 250;  
      average2 = sum2 / 250;  
      Serial.print(average1, 3);
      Serial.print(", ");
      Serial.println(average2, 3);
      sum1 = 0.0;
      sum2 = 0.0;
      count = 0;
    }
  }