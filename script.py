import serial
import paho.mqtt.client as mqtt

# --- CONFIG ---
SERIAL_PORT = 'COM7' # Check your Arduino IDE for the correct port
BAUD_RATE = 115200
BROKER_IP = "localhost" # It sends to itself
TOPIC = "infra/sensors"

ser = serial.Serial(SERIAL_PORT, BAUD_RATE)
client = mqtt.Client()
client.connect(BROKER_IP, 1883)

print("Bridge Active: Sending USB data to Local Broker...")

while True:
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').strip()
        client.publish(TOPIC, line)