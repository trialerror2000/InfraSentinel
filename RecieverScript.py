import paho.mqtt.client as mqtt
import json

# --- CONFIGURATION ---
# Replace this with Laptop A's IPv4 Address (e.g., "192.168.1.15")
BROKER_IP = "192.168.1.XX" 
TOPIC = "infra/sensors"

# Callback when we connect to the broker
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print(f"Connected successfully to Broker at {BROKER_IP}")
        client.subscribe(TOPIC)
        print(f"Subscribed to topic: {TOPIC}\n" + "-"*40)
    else:
        print(f"Connection failed with code {rc}")

# Callback when a message is received
def on_message(client, userdata, msg):
    try:
        # Decode the JSON payload
        payload = msg.payload.decode("utf-8")
        data = json.loads(payload)
        
        # Clean terminal output of the incoming data structure
        print(f"Node 1 (Moisture): {'ACTIVE' if data['n1_active'] else 'OFFLINE'} | Val: {data['moisture']}")
        print(f"Node 2 (Tilt):     {'ACTIVE' if data['n2_active'] else 'OFFLINE'} | X: {data['tilt_x']}, Y: {data['tilt_y']}")
        print(f"Node 3 (Strain):   {'ACTIVE' if data['n3_active'] else 'OFFLINE'} | Val: {data['strain']}")
        print("-" * 40)
        
    except Exception as e:
        print(f"Error parsing data: {e} | Raw: {msg.payload}")

# Initialize Client
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

print(f"Attempting to connect to Laptop A ({BROKER_IP})...")
try:
    client.connect(BROKER_IP, 1883, 60)
    client.loop_forever() # Keep the script running to listen for data
except Exception as e:
    print(f"Could not connect: {e}")