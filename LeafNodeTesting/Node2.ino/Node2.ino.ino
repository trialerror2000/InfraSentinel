#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>

#define NODE_ID 2
#define NEO_PIN D4

uint8_t receiverAddress[] = {0xC0, 0x5D, 0x89, 0xB1, 0x1C, 0x24};
Adafruit_MPU6050 mpu;
typedef struct struct_message { int nodeID; float rawVal1; float rawVal2; } struct_message;
struct_message myData;
Adafruit_NeoPixel statusLED(1, NEO_PIN, NEO_GRB + NEO_KHZ800);
bool successFlag = false;

void OnDataSent(uint8_t *mac, uint8_t status) { successFlag = (status == 0); }

void setup() {
  statusLED.begin();
  mpu.begin(); // Standard I2C: D1 (SCL), D2 (SDA)
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(receiverAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  myData.nodeID = NODE_ID;
  myData.rawVal1 = a.acceleration.x; 
  myData.rawVal2 = a.acceleration.y;

  esp_now_send(receiverAddress, (uint8_t *) &myData, sizeof(myData));

  if (successFlag) {
    statusLED.setPixelColor(0, statusLED.Color(0, 255, 0));
    statusLED.show(); delay(500);
  } else {
    statusLED.setPixelColor(0, statusLED.Color(255, 0, 0));
    statusLED.show(); delay(150);
    statusLED.setPixelColor(0, 0);
    statusLED.show(); delay(150);
  }
}