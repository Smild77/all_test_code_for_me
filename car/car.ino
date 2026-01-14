#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


const char* ssid = "PL";
const char* password = "00000000";

WiFiUDP udp;
unsigned int udpPort = 4210;
char packetBuffer[512];

void sendBeacon();
void collectRSSIData();
void receiveData();
void sendCrossReference(int remoteId, int rssi);
int estimateRSSI(int packetSize, unsigned long receiveTime);
void checkConnectivity();

// ✅ ESP1 (Bottom-Left Corner)
const int SENSOR_ID = 3;
const float SENSOR_X = 10.0;
const float SENSOR_Y = 90.0;

unsigned long previousScan = 0;
unsigned long previousBeacon = 0;
unsigned long previousConnCheck = 0;
const long SCAN_INTERVAL = 2000;
const long BEACON_INTERVAL = 1000;
const long CONN_CHECK_INTERVAL = 10000;

struct ReceivedPacket {
  unsigned long timestamp;
  int size;
  IPAddress source;
};

ReceivedPacket lastPackets[10];
int packetIndex = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP1 Bottom-Left Corner Node ===");
  Serial.println("🔲 2D Indoor Positioning System");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\n✅ ESP1 connected successfully!");
  Serial.printf("📍 Position: Bottom-Left (%.1f, %.1f)\n", SENSOR_X, SENSOR_Y);
  Serial.printf("🌐 IP Address: %s\n", WiFi.localIP().toString().c_str());
  Serial.printf("📡 MAC Address: %s\n", WiFi.macAddress().c_str());
  
  udp.begin(udpPort);
  Serial.printf("🔊 UDP listening on port %d\n", udpPort);
  Serial.println("🚀 Bottom-Left node ready!\n");
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousConnCheck >= CONN_CHECK_INTERVAL) {
    previousConnCheck = currentMillis;
    checkConnectivity();
  }
  
  if (currentMillis - previousBeacon >= BEACON_INTERVAL) {
    previousBeacon = currentMillis;
    sendBeacon();
  }
  
  if (currentMillis - previousScan >= SCAN_INTERVAL) {
    previousScan = currentMillis;
    collectRSSIData();
  }
  
  receiveData();
  delay(50);
}

void checkConnectivity() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi disconnected! Reconnecting...");
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n✅ WiFi reconnected!");
    } else {
      Serial.println("\n❌ WiFi reconnection failed!");
    }
  } else {
    Serial.printf("📶 ESP1 WiFi OK - RSSI: %d dBm\n", WiFi.RSSI());
  }
}

void sendBeacon() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  JsonDocument beacon;
  beacon["type"] = "beacon";
  beacon["sensor_id"] = SENSOR_ID;
  beacon["x"] = SENSOR_X;
  beacon["y"] = SENSOR_Y;
  beacon["timestamp"] = millis();
  beacon["mac"] = WiFi.macAddress();
  beacon["wifi_rssi"] = WiFi.RSSI();
  beacon["corner"] = "bottom-left";
  
  String beaconString;
  serializeJson(beacon, beaconString);
  
  IPAddress broadcastIP(255, 255, 255, 255);
  udp.beginPacket(broadcastIP, udpPort);
  udp.write(beaconString.c_str());
  udp.endPacket();
  
  Serial.printf("📡 Beacon sent from ESP1 Bottom-Left (%.1f, %.1f) RSSI: %d dBm\n", 
                SENSOR_X, SENSOR_Y, WiFi.RSSI());
}

void collectRSSIData() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  JsonDocument doc;
  doc["type"] = "rssi_data";
  doc["sensor_id"] = SENSOR_ID;
  doc["x"] = SENSOR_X;
  doc["y"] = SENSOR_Y;
  doc["timestamp"] = millis();
  doc["uptime"] = millis() / 1000;
  doc["corner"] = "bottom-left";
  doc["self_rssi"] = WiFi.RSSI();
  doc["wifi_status"] = WiFi.status();
  doc["free_heap"] = ESP.getFreeHeap();
  
  String dataString;
  serializeJson(doc, dataString);
  
  IPAddress serverIP(192, 168, 1, 100);
  udp.beginPacket(serverIP, udpPort);
  udp.write(dataString.c_str());
  udp.endPacket();
  
  Serial.printf("📊 RSSI data sent from ESP1 (%.1f, %.1f): %d dBm\n", 
                SENSOR_X, SENSOR_Y, WiFi.RSSI());
}

void receiveData() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    unsigned long receiveTime = millis();
    IPAddress remoteIP = udp.remoteIP();
    
    int len = udp.read(packetBuffer, 512);
    if (len > 0) {
      packetBuffer[len] = 0;
      
      lastPackets[packetIndex] = {receiveTime, packetSize, remoteIP};
      packetIndex = (packetIndex + 1) % 10;
      
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, packetBuffer);
      
      if (!error) {
        if (doc["type"] == "beacon") {
          int remote_id = doc["sensor_id"];
          if (remote_id != SENSOR_ID && remote_id >= 1 && remote_id <= 5) {
            int estimated_rssi = estimateRSSI(packetSize, receiveTime);
            String corner = doc["corner"] | "unknown";
            float remote_x = doc["x"];
            float remote_y = doc["y"];
            
            Serial.printf("📨 Received beacon from ESP%d %s (%.1f, %.1f) Est. RSSI: %d dBm\n", 
                         remote_id, corner.c_str(), remote_x, remote_y, estimated_rssi);
            
            sendCrossReference(remote_id, estimated_rssi);
          }
        }
      }
    }
  }
}

int estimateRSSI(int packetSize, unsigned long receiveTime) {
  int baseRSSI = WiFi.RSSI();
  int sizeAdjustment = (packetSize < 100) ? -5 : (packetSize > 200) ? 5 : 0;
  
  static unsigned long lastReceiveTime = 0;
  int timingAdjustment = 0;
  if (lastReceiveTime > 0) {
    unsigned long timeDiff = receiveTime - lastReceiveTime;
    if (timeDiff < 800) {
      timingAdjustment = 3;
    } else if (timeDiff > 1200) {
      timingAdjustment = -3;
    }
  }
  lastReceiveTime = receiveTime;
  
  int randomNoise = random(-3, 4);
  int estimatedRSSI = baseRSSI + sizeAdjustment + timingAdjustment + randomNoise;
  
  return constrain(estimatedRSSI, -90, -30);
}

void sendCrossReference(int remoteId, int rssi) {
  if (WiFi.status() != WL_CONNECTED) return;
  
  JsonDocument doc;
  doc["type"] = "cross_rssi";
  doc["from_sensor"] = SENSOR_ID;
  doc["to_sensor"] = remoteId;
  doc["rssi"] = rssi;
  doc["timestamp"] = millis();
  doc["measurement_method"] = "estimated";
  doc["wifi_rssi"] = WiFi.RSSI();
  doc["from_corner"] = "bottom-left";
  doc["from_x"] = SENSOR_X;
  doc["from_y"] = SENSOR_Y;
  
  String dataString;
  serializeJson(doc, dataString);
  
  IPAddress serverIP(192, 168, 1, 100);
  udp.beginPacket(serverIP, udpPort);
  udp.write(dataString.c_str());
  udp.endPacket();
  
  Serial.printf("📤 Cross RSSI sent: ESP1(%.1f,%.1f)→ESP%d, RSSI: %d dBm\n", 
               SENSOR_X, SENSOR_Y, remoteId, rssi);
}
