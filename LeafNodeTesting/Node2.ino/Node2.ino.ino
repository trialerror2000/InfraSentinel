#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Adafruit_NeoPixel.h>

#define NODE_ID 2
#define NEO_PIN D4

uint8_t receiverAddress[] = {0xC0, 0x5D, 0x89, 0xB1, 0x1C, 0x24};

typedef struct struct_message {
    int nodeID;
    char sensorData[32];
    int statusCode;
} struct_message;

struct_message myData;
Adafruit_NeoPixel statusLED(1, NEO_PIN, NEO_GRB + NEO_KHZ800);
bool deliverySuccess = false;

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  deliverySuccess = (sendStatus == 0);
}

void setup() {
  Serial.begin(115200);
  statusLED.begin();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) return;
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(receiverAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
}

void loop() {
  myData.nodeID = NODE_ID;
  strcpy(myData.sensorData, "No Data");
  myData.statusCode = 0;

  esp_now_send(receiverAddress, (uint8_t *) &myData, sizeof(myData));

  if (!deliverySuccess) {
    statusLED.setPixelColor(0, statusLED.Color(255, 0, 0)); // Blink RED
    statusLED.show(); delay(200);
    statusLED.setPixelColor(0, statusLED.Color(0, 0, 0));
    statusLED.show(); delay(200);
  } else {
    statusLED.setPixelColor(0, statusLED.Color(0, 255, 0)); // Solid GREEN
    statusLED.show();
    delay(1000);
  }
}