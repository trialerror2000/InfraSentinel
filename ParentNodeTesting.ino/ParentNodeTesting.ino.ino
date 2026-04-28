#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_NeoPixel.h>

// --- PIN DEFINITIONS (RIGHT SIDE ONLY) ---
#define NEO_PIN     19    // Data pin for 3-LED chain
#define BUZZ_PIN    18    // Signal pin for Passive Buzzer
#define OLED1_SDA   21
#define OLED1_SCL   22
#define OLED2_SDA   17
#define OLED2_SCL   16

// --- OBJECTS & SETTINGS ---
#define NUM_LEDS    3
Adafruit_NeoPixel statusLEDs(NUM_LEDS, NEO_PIN, NEO_GRB + NEO_KHZ800);

TwoWire I2C_Bus1 = TwoWire(0); // Hardware Bus 0
TwoWire I2C_Bus2 = TwoWire(1); // Hardware Bus 1

Adafruit_SH1106G display1 = Adafruit_SH1106G(128, 64, &I2C_Bus1, -1);
Adafruit_SH1106G display2 = Adafruit_SH1106G(128, 64, &I2C_Bus2, -1);

void setup() {
  Serial.begin(115200);
  
  // 1. Initialize LEDs & Buzzer Pin
  statusLEDs.begin();
  statusLEDs.setBrightness(40);
  statusLEDs.show(); // All off
  
  pinMode(BUZZ_PIN, OUTPUT); // New requirement for Core 3.0

  // 2. Startup Sound & Light
  Serial.println("System Booting...");
  for(int i=0; i<NUM_LEDS; i++) {
    statusLEDs.setPixelColor(i, statusLEDs.Color(0, 0, 255)); // Blue for boot
    statusLEDs.show();
    toneChirp(2000 + (i*500), 100);
    delay(100);
  }

  // 3. Initialize I2C Displays
  I2C_Bus1.begin(OLED1_SDA, OLED1_SCL);
  I2C_Bus2.begin(OLED2_SDA, OLED2_SCL);

  if(!display1.begin(0x3C, true)) Serial.println("OLED 1 (Node 1) failed!");
  if(!display2.begin(0x3C, true)) Serial.println("OLED 2 (Nodes 2/3) failed!");

  // 4. Initial Screen Draw
  refreshDashboard();
  
  // Change LEDs to Green to show ready
  setAllLEDs(0, 255, 0); 
  toneChirp(3000, 300);
  Serial.println("System Ready.");
}

void loop() {
  // Simple "Heartbeat" pulse on the LEDs
  static int brightness = 40;
  static bool fading = true;
  
  statusLEDs.setBrightness(brightness);
  statusLEDs.show();
  
  if(fading) brightness--; else brightness++;
  if(brightness <= 10 || brightness >= 60) fading = !fading;
  
  delay(30);
}

// --- HELPER FUNCTIONS ---

// Updated to use the new standard tone() function
void toneChirp(int freq, int dur) {
  tone(BUZZ_PIN, freq, dur);
  delay(dur + 10); // Slight pause to let the tone finish
}

void setAllLEDs(int r, int g, int b) {
  for(int i=0; i<NUM_LEDS; i++) {
    statusLEDs.setPixelColor(i, statusLEDs.Color(r, g, b));
  }
  statusLEDs.show();
}

void refreshDashboard() {
  // DISPLAY 1: PRIMARY MONITOR (Node 1)
  display1.clearDisplay();
  display1.setTextSize(1);
  display1.setTextColor(SH110X_WHITE);
  display1.setCursor(0,0);
  display1.println("PRIMARY MONITOR: N1");
  display1.drawFastHLine(0, 10, 128, SH110X_WHITE);
  
  display1.setCursor(0, 20);
  display1.println("DEFORMATION: 0.00 deg");
  display1.setCursor(0, 35);
  display1.println("MOISTURE: 0%");
  display1.setCursor(0, 55);
  display1.println("STATUS: [SCANNING]");
  display1.display();

  // DISPLAY 2: AUXILIARY MONITOR (Nodes 2 & 3)
  display2.clearDisplay();
  display2.setTextSize(1);
  display2.setTextColor(SH110X_WHITE);
  display2.setCursor(0,0);
  display2.println("NODE 2        NODE 3");
  display2.drawFastHLine(0, 10, 128, SH110X_WHITE);
  display2.drawFastVLine(64, 12, 52, SH110X_WHITE); // Center Divider

  // Node 2 Column
  display2.setCursor(0, 20);
  display2.println("DIST: --mm");
  display2.setCursor(0, 40);
  display2.println("VIBE: SAFE");

  // Node 3 Column
  display2.setCursor(70, 20);
  display2.println("CRACK: 0mm");
  display2.setCursor(70, 40);
  display2.println("ATM: 1013 hPa");

  display2.display();
}