#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);
  
  Serial.println("\n---------------------------------");
  Serial.println("ESP32 PARENT NODE MAC ADDRESS:");
  Serial.println(WiFi.macAddress());
  Serial.println("---------------------------------");
  Serial.println("Address needed for Leaf Node Configuration");
}

void loop() {
   
}