import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time
import re
import numpy as np
import os
import csv
import threading
import pandas as pd

# Function to listen for Enter key
def stop_recording():
    global recording
    input("Press Enter to stop recording...\n")
    recording = False

# Input your threshold value here
threshold = 0.5

# Replace '/dev/ttyUSB0' with the correct port for your Pico
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
ser.flushInput()  # Clear any old data
time.sleep(2)  # Give time for connection to stabilize
print("Connected to:", ser.name)  # Debugging
print("Press Enter to start recording...")
input()  # Wait for user confirmation
start_time = time.time()
recording = True
print("Recording... Press Enter to stop.")
stop_thread = threading.Thread(target=stop_recording)
stop_thread.start()

# Directory to save outputs
OUTPUT_DIR = "recorded_data"
os.makedirs(OUTPUT_DIR, exist_ok=True)

timestamp = time.strftime("%Y%m%d_%H%M%S")
csv_filename = os.path.join(OUTPUT_DIR, f"serial_data_{timestamp}.csv")
plot_filename = os.path.join(OUTPUT_DIR, f"serial_plot_{timestamp}.png")

csv_file = open(csv_filename, "w", newline="")
csv_writer = csv.writer(csv_file)
csv_writer.writerow(["Time", "LeadPosition", "FollowPosition", "uLead", "uFollow"]) 


while recording:
    try:
        line_data = ser.readline().decode().strip()  # Read from Pico
        
        if line_data:
            # Extract data
            numbers = re.findall(r"[-+]?\d*\.\d+|\d+", line_data)
            motor1_position, motor2_position, _, command1, command2, _, _, _, _, _ = list(map(float, numbers))

            # time_str = time.strftime("%H:%M:%S")
            elapsed_time = round(time.time() - start_time,2)
            csv_writer.writerow([elapsed_time, motor1_position, motor2_position, command1, command2])
            print(f"{elapsed_time}, {line_data}")
    
    except Exception as e:
        print(f"Error: {e}")
    
ser.close()
print("Serial connection closed.")

csv_file.close()
print(f"Data saved in {csv_filename}")

df = pd.read_csv(csv_filename)

fig, axs = plt.subplots(2, 1, figsize=(8, 12))
plt.subplots_adjust(hspace=0.4)

threshold_line = threshold * np.ones_like(df.index)
max_threshold = np.ones_like(df.index)

# === Update Subplot 1: Motor Positions ===
axs[0].plot(df['Time'], df['LeadPosition'], label="Motor 1 Position", color='b')
axs[0].plot(df['Time'], df['FollowPosition'], label="Motor 2 Position", color='r')
axs[0].set_ylabel("Position(mm)")
axs[0].set_title("Motor Positions Over Time")
axs[0].legend()
axs[0].grid(True)

# === Update Subplot 2: Motor Commands ===
axs[1].plot(df['Time'], df['uLead'], label="Motor 1 Command", color='b')
axs[1].plot(df['Time'], df['uFollow'], label="Motor 2 Command", color='r')
axs[1].plot(df['Time'], threshold_line, label="Motor 1 Command Threshold", color='g')
axs[1].plot(df['Time'], -threshold_line, color='g')
axs[1].plot(df['Time'], max_threshold, label="Max Command Threshold", color='black')
axs[1].plot(df['Time'], -max_threshold, color='black')
axs[1].set_xlabel("Time(s)")
axs[1].set_ylabel("Command Value")
axs[1].set_title("Motor Commands Over Time")
axs[1].legend()
axs[1].grid(True)

fig.savefig(plot_filename)
print(f"Plot saved as {plot_filename}")
plt.show()  # Show final plot