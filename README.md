# InfraSentinel
Geotechnical Wireless Sensor Networks for Continuous Slope Stability

# Infrastructure Sentinels: Core Architecture & Alert Logic

## 🏗️ System Topology
The system operates on a localized, off-grid **Cluster-Tree Topology** using the ESP-NOW protocol. It consists of three distributed edge nodes (The Sentinels) streaming continuous telemetry to a centralized command hub (The Parent Node).

### 1. The Parent Node (Cluster Command Center)
The Parent Node acts as the centralized aggregation and physical alerting hub. It processes incoming data packets and translates them into immediate visual and acoustic warnings.
* **Microcontroller:** ESP32 WROOM-32
* **Role:** ESP-NOW Receiver & Alert Coordinator
* **Visual Dashboards:** Dual independent 0.96" OLED displays.
    * *Display 1:* Dedicated primary monitor for Node 1.
    * *Display 2:* Split-screen auxiliary monitor for Nodes 2 & 3.
* **Status Bar:** 3-LED WS2812B NeoPixel chain (physically mapped left-to-right for Nodes 1, 2, and 3).
* **Acoustic Warning:** Passive Buzzer utilizing hardware PWM for frequency-modulated sirens.

### 2. The Leaf Nodes (The Sentinels)
Three distributed edge modules designed for ultra-low latency data transmission. 
* **Microcontrollers:** ESP8266 (Wemos D1 Mini / NodeMCU)
* **Role:** ESP-NOW Transmitters
* **Visual Indicator:** 1x WS2812B NeoPixel per node for localized status verification.

---

## 🚦 System Operating States & LED Logic
The system relies on a strict, unified color-coding standard across both the Parent Node's status bar and the localized Leaf Node LEDs.

| Status Level | LED Color | Parent Node Buzzer Action | System Condition |
| :--- | :--- | :--- | :--- |
| **Boot / Init** | **Blue** | Ascending Chime Sweep | System powering up; initializing radios. |
| **Disconnected** | **Red (Static)** | Silent | Node offline, out of range, or awaiting initial ESP-NOW handshake. |
| **Stable** | **Green** | Silent | Node connected; structural parameters are within nominal safety thresholds. |
| **Warning** | **Yellow** | Slow Beep (1000 Hz) | Minor threshold deviation detected; high-frequency sampling initiated. |
| **Critical** | **Red** | Fast Siren (3500 Hz) | Catastrophic threshold breached; immediate evacuation protocol required. |

Progress Till now........

<img width="1280" height="960" alt="SampleUI" src="https://github.com/user-attachments/assets/c6289eb2-82f3-4e8b-a434-5888b2619f81" />

Leaf Nodes 

<img width="960" height="1280" alt="image" src="https://github.com/user-attachments/assets/56200950-b772-44ed-9427-0db48c5df177" />

