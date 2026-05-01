import streamlit as st
import pandas as pd
import paho.mqtt.client as mqtt
import json
import time

# --- STREAMLIT PAGE CONFIG ---
st.set_page_config(page_title="Infra Sentinel Live", layout="wide")
st.title("🛰️ Infra Sentinel: Real-Time Slope Analytics")

# 1. Initialize Persistent Data Storage
if 'sensor_buffer' not in st.session_state:
    # Creating a buffer to store the last 50 readings
    st.session_state.sensor_buffer = pd.DataFrame(
        columns=['timestamp', 'moisture', 'tilt_x', 'tilt_y', 'strain']
    )

# 2. MQTT Callbacks
def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode("utf-8"))
        
        # Create a new data row from the JSON payload
        new_row = {
            'timestamp': pd.Timestamp.now(),
            'moisture': payload['moisture'],
            'tilt_x': payload['tilt_x'],
            'tilt_y': payload['tilt_y'],
            'strain': payload['strain']
        }
        
        # Append to the session state buffer
        new_df = pd.DataFrame([new_row])
        st.session_state.sensor_buffer = pd.concat(
            [st.session_state.sensor_buffer, new_df]
        ).tail(50) # Keep only the most recent 50 points
        
    except Exception as e:
        pass # Handle parsing errors silently for the demo

# 3. Setup Background MQTT Client (Run once)
if 'mqtt_client' not in st.session_state:
    client = mqtt.Client()
    client.on_message = on_message
    # Use Laptop A's IP Address here
    client.connect("192.168.1.XX", 1883, 60) 
    client.subscribe("infra/sensors")
    client.loop_start() # This runs in a BACKGROUND thread
    st.session_state.mqtt_client = client

# --- UI LAYOUT ---

# Placeholders for dynamic content
col1, col2, col3 = st.columns(3)
with col1:
    moisture_stat = st.empty()
with col2:
    tilt_stat = st.empty()
with col3:
    strain_stat = st.empty()

chart_placeholder = st.empty()

# --- THE UI REFRESH LOOP ---
# This is the ONLY "While True" allowed in Streamlit, 
# and it MUST include a small sleep.
while True:
    df = st.session_state.sensor_buffer
    
    if not df.empty:
        latest = df.iloc[-1]
        
        # Update Big Metrics
        moisture_stat.metric("Soil Moisture", f"{latest['moisture']}%")
        tilt_stat.metric("Tilt (X-Axis)", f"{latest['tilt_x']}°")
        strain_stat.metric("Structural Strain", f"{latest['strain']}")

        # Update Live Chart
        # We set 'timestamp' as index so the X-axis shows time
        chart_placeholder.line_chart(df.set_index('timestamp')[['moisture', 'tilt_x', 'strain']])
    
    time.sleep(0.1) # Small delay to prevent CPU spiking