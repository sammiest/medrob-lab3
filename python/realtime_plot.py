import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time
import re
import numpy as np

# Input your threshold value here
threshold = 0

# Replace '/dev/ttyUSB0' with the correct port for your Pico
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=2)
ser.flushInput()  # Clear any old data
time.sleep(2)  # Give time for connection to stabilize
print("Connected to:", ser.name)  # Debugging


# Initialize plot
states = np.zeros((1,4))
# states = np.zeros((1,5))
x_index = []

plt.ion()
fig, axs = plt.subplots(2, 1, figsize=(8, 6))
plt.subplots_adjust(hspace=0.4)

while True:
    try:
        line_data = ser.readline().decode().strip()  # Read from Pico
        
        if line_data:
            # Extract data
            numbers = re.findall(r"[-+]?\d*\.\d+|\d+", line_data)
            motor1_position, motor2_position, _, command1, command2, _, _, _, _, _ = list(map(float, numbers))

            # Record state
            cur_state = np.array([motor1_position, motor2_position, command1, command2])
            states = np.vstack((states, cur_state))
            x_index.append(states.shape[0] - 1)  # Simple index-based X-axis
            
            if len(x_index) == 1:
                states = states[1:,:]
            # Keep the last 100 points
            x_index = x_index[-100:]

            # Set threshold
            threshold_line = threshold * np.ones_like(x_index)
            max_threshold = np.ones_like(x_index)

            # === Update Subplot 1: Motor Positions ===
            axs[0].cla()
            axs[0].plot(x_index, states[-100:,0], label="Motor 1 Position", color='b')
            axs[0].plot(x_index, states[-100:,1], label="Motor 2 Position", color='r')
            axs[0].set_ylabel("Position")
            axs[0].set_title("Motor Positions Over Time")
            axs[0].legend()
            axs[0].grid(True)

            # === Update Subplot 2: Motor Commands ===
            axs[1].cla()
            axs[1].plot(x_index, states[-100:,2], label="Motor 1 Command", color='b')
            axs[1].plot(x_index, states[-100:,3], label="Motor 2 Command", color='r')
            axs[1].plot(x_index, threshold_line, label="Motor 1 Command Threshold", color='g')
            axs[1].plot(x_index, -threshold_line, color='g')
            axs[1].plot(x_index, max_threshold, label="Max Command Threshold", color='black')
            axs[1].plot(x_index, -max_threshold, color='black')
            axs[1].set_xlabel("Time (s)")
            axs[1].set_ylabel("Command Value")
            axs[1].set_title("Motor Commands Over Time")
            axs[1].legend()
            axs[1].grid(True)

            plt.pause(0.001)  # Force update without freezing
    except Exception as e:
        print(f"Error: {e}")