#include <Arduino.h>

unsigned long resetInterval = 3600000; // ตั้งเวลา 1 ชั่วโมง (หน่วย: มิลลิวินาที)     1 นาที = 60000 30 นาที = 1800000 1 ชั่วโมง = 3600000
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("เริ่มต้นระบบ...");
  previousMillis = millis(); // บันทึกเวลาเริ่มต้น
}

void loop() {
  unsigned long currentMillis = millis();
  
  // ตรวจสอบว่าถึงเวลารีเซ็ตหรือยัง
  if (currentMillis - previousMillis >= resetInterval) {
    Serial.println("ถึงเวลารีเซ็ตแล้ว!");
    delay(100); // ให้ Serial Monitor ส่งข้อมูลเสร็จก่อน
    ESP.restart(); // รีเซ็ตบอร์ด
  }

  // โค้ดอื่นๆ ที่ทำงานปกติ
  Serial.println("กำลังทำงาน...");
  delay(1000);
}