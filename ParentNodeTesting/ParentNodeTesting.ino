#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_NeoPixel.h>

// --- PIN DEFINITIONS ---
#define NEO_PIN     19
#define BUZZ_PIN    18
#define OLED1_SDA   21
#define OLED1_SCL   22
#define OLED2_SDA   17
#define OLED2_SCL   16

// --- HARDWARE OBJECTS ---
#define NUM_LEDS    3
Adafruit_NeoPixel statusLEDs(NUM_LEDS, NEO_PIN, NEO_GRB + NEO_KHZ800);

TwoWire I2C_Bus1 = TwoWire(0);
TwoWire I2C_Bus2 = TwoWire(1);
Adafruit_SH1106G display1 = Adafruit_SH1106G(128, 64, &I2C_Bus1, -1);
Adafruit_SH1106G display2 = Adafruit_SH1106G(128, 64, &I2C_Bus2, -1);

// --- ESP-NOW DATA STRUCTURE ---
typedef struct struct_message {
    int nodeID;
    float rawVal1; 
    float rawVal2;
} struct_message;

struct_message n1_data, n2_data, n3_data;

// --- HEARTBEAT TRACKING LOGIC ---
unsigned long lastRecv1 = 0;
unsigned long lastRecv2 = 0;
unsigned long lastRecv3 = 0;
const unsigned long TIMEOUT_MS = 2500; // 2.5 seconds without data = Offline

// --- ESP-NOW CALLBACK ---
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *data, int len) {
  struct_message incoming;
  memcpy(&incoming, data, sizeof(incoming));
  
  if (incoming.nodeID == 1) {
    n1_data = incoming;
    lastRecv1 = millis(); // Reset the timer for Node 1
  } 
  else if (incoming.nodeID == 2) {
    n2_data = incoming;
    lastRecv2 = millis(); // Reset the timer for Node 2
  } 
  else if (incoming.nodeID == 3) {
    n3_data = incoming;
    lastRecv3 = millis(); // Reset the timer for Node 3
  }
}

void setup() {
  Serial.begin(115200);

  // 1. Initialize Outputs
  pinMode(BUZZ_PIN, OUTPUT);
  statusLEDs.begin();
  statusLEDs.setBrightness(40);
  statusLEDs.show();

  // 2. The Rainbow Boot Ritual (Synced with Buzzer)
  for(int i = 0; i < 3; i++) {
    tone(BUZZ_PIN, 2000 + (i * 500), 150);
    for(int j=0; j<256; j+=85) {
      for(int k=0; k<NUM_LEDS; k++) {
        statusLEDs.setPixelColor(k, statusLEDs.ColorHSV((j + (k*85)) * 256, 255, 255));
      }
      statusLEDs.show();
      delay(50); 
    }
  }
  
  // Final Confirmation Tone
  tone(BUZZ_PIN, 3000, 300);
  for(int k=0; k<NUM_LEDS; k++) statusLEDs.setPixelColor(k, statusLEDs.Color(255, 255, 255)); 
  statusLEDs.show();
  delay(350);

  // Instantly set to RED (Disconnected state)
  for(int i=0; i<NUM_LEDS; i++) statusLEDs.setPixelColor(i, statusLEDs.Color(255, 0, 0));
  statusLEDs.show();

  // 3. Initialize Displays
  I2C_Bus1.begin(OLED1_SDA, OLED1_SCL);
  I2C_Bus2.begin(OLED2_SDA, OLED2_SCL);
  display1.begin(0x3C, true); 
  display2.begin(0x3C, true);

  // 4. Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Fail");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  unsigned long now = millis();

  // --- HEARTBEAT CHECK ---
  // A node is connected if its lastRecv timer isn't 0 AND the time since the last packet is less than TIMEOUT_MS
  bool n1_connected = (lastRecv1 != 0) && (now - lastRecv1 < TIMEOUT_MS);
  bool n2_connected = (lastRecv2 != 0) && (now - lastRecv2 < TIMEOUT_MS);
  bool n3_connected = (lastRecv3 != 0) && (now - lastRecv3 < TIMEOUT_MS);

  // --- LED CONNECTION STATUS LOGIC ---
  statusLEDs.setPixelColor(0, n1_connected ? statusLEDs.Color(0, 255, 0) : statusLEDs.Color(255, 0, 0));
  statusLEDs.setPixelColor(1, n2_connected ? statusLEDs.Color(0, 255, 0) : statusLEDs.Color(255, 0, 0));
  statusLEDs.setPixelColor(2, n3_connected ? statusLEDs.Color(0, 255, 0) : statusLEDs.Color(255, 0, 0));
  statusLEDs.show();

  // --- SCREEN 1: NODE 2 (MPU6050 TILT) ---
  display1.clearDisplay();
  display1.setTextSize(1);
  display1.setTextColor(SH110X_WHITE);
  display1.setCursor(0,0);
  display1.println("MPU6050: SLOPE");
  display1.drawFastHLine(0, 10, 128, SH110X_WHITE);
  
  if(n2_connected) {
    display1.setCursor(0, 25);
    display1.print("X-AXIS: "); display1.println(n2_data.rawVal2, 2);
    display1.setCursor(0, 45);
    display1.print("Y-AXIS: "); display1.println(n2_data.rawVal1, 2);
  } else {
    display1.setTextSize(2);
    display1.setCursor(15, 30);
    display1.println("OFFLINE");
  }
  display1.display();

  // --- SCREEN 2: NODE 1 (MOISTURE) & NODE 3 (STRAIN) ---
  display2.clearDisplay();
  display2.setTextSize(1);
  display2.setTextColor(SH110X_WHITE);
  display2.setCursor(0,0);
  display2.println("Moisture     Strain: ");
  display2.drawFastHLine(0, 10, 128, SH110X_WHITE);
  display2.drawFastVLine(64, 12, 52, SH110X_WHITE); // Center Divider

  // Left Column (Node 1)
  display2.setCursor(0, 30);
  if(n1_connected) {
    display2.print("VAL:"); display2.print((int)n1_data.rawVal1);
  } else {
    display2.print("OFFLINE");
  }

  // Right Column (Node 3)
  display2.setCursor(70, 30);
  if(n3_connected) {
    display2.print("VAL:"); display2.print((int)n3_data.rawVal1);
  } else {
    display2.print("OFFLINE");
  }
  display2.display();

  // ---------------------------------------------------------
  // NEW ADDITION: JSON SERIAL OUTPUT FOR PYTHON/MQTT BRIDGE
  // ---------------------------------------------------------
  String jsonData = "{";
  jsonData += "\"n1_active\":" + String(n1_connected) + ",";
  jsonData += "\"moisture\":" + (n1_connected ? String(n1_data.rawVal1, 2) : "0") + ",";
  
  jsonData += "\"n2_active\":" + String(n2_connected) + ",";
  jsonData += "\"tilt_x\":" + (n2_connected ? String(n2_data.rawVal2, 2) : "0") + ",";
  jsonData += "\"tilt_y\":" + (n2_connected ? String(n2_data.rawVal1, 2) : "0") + ",";
  
  jsonData += "\"n3_active\":" + String(n3_connected) + ",";
  jsonData += "\"strain\":" + (n3_connected ? String(n3_data.rawVal1, 2) : "0");
  jsonData += "}";

  Serial.println(jsonData);
  // ---------------------------------------------------------

  delay(100); 
}