#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\nTurning on Wi-Fi Radio...");
  WiFi.mode(WIFI_STA);
  
  // THE FIX: Wait until the MAC is no longer zeroes
  String mac = WiFi.macAddress();
  while (mac == "00:00:00:00:00:00") {
    Serial.print(".");
    delay(100);
    mac = WiFi.macAddress();
  }
  
  Serial.println("\n\n---------------------------------");
  Serial.println("ESP32 PARENT NODE MAC ADDRESS:");
  Serial.println(mac);
  Serial.println("---------------------------------");
}
void loop() {
  // Do nothing
}