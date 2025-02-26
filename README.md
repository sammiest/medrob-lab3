# WN25 ROB599 MedRob Lab3

## Table of Content

- [Description](#description)
- [Linux Setup](#linux-setup)
- [Installation](#installation)
- [Visualize Serial Output](#visualize-serial-output)
- [Plotting(Real-time)](#plotting-real-time)
- [Plotting(Post-process)]
- [(Optional) GitHub SSH Setup](/optional_ssh_setup.md)

## Description
This repo is designed for Lab 3 of ROB599 Medical Robotics, WN2025 University of Michigan. It is to be deployed on the Raspberry Pi Pico tailored specifically for MBot, some originates from [Mbot Firmware GitHub Page](https://github.com/mbot-project/mbot_firmware/releases)

For the coding part of this lab, you should only revise [`mbot_teleoperation.c`](/tests/mbot_teleoperation.c)

## Linux Setup
The [MBot Pico SDK](https://github.com/MBot-Project-Development/pico_sdk) is simplified, and Linux-based only. You are welcomed to use any Linux-based distribution or methods such as a virtual machine or dual boot. However, for simplicity, we provide a WSL setup guide here. Refer to [Microsoft's Official WSL Installation](https://learn.microsoft.com/en-us/windows/wsl/install) for more info. 

For Mac users, you can explore using **brew** and **Clang**, or please find a Windows teammate ( ಠ‿<)

Open PowerShell or Windows Command Prompt in **administrator** mode
```powershell
# To install default Ubuntu distribution
wsl --install

# To check all available distributions and install a specific one
wsl --list --online
wsl --install -d <Distribution Name> # It's tested on Ubuntu-22.04
```
Then open the distribution using the Start menu. You will be asked to create a **User Name** and **Password** for your Linux distribution. This account will be considered the Linux administrator, with the ability to run `sudo` administrative commands, so remember your **Password**!

If you see something like this when you open a PowerShell, you are good to go.

 ![PS](media/PS.png)

## Installation
1. We recommand forking this repo into your own GitHub account for your own version control
    ```bash
    # Check if Git installed locally
    git --version
    # If not
    sudo apt-get install git

    # Then clone this repo in your desired local folder
    git clone https://github.com/<github_username>/medrob-lab3.git
    ```

1. After cloning the repository, run the setup script:
    ```bash
    cd ~/medrob-lab3
    ./setup.sh
    ```
    - which will install dependencies (requires sudo password) and initialize the submodules.

    If setup.sh is not executable by default, run the following to enable it:

    ```bash
    sudo chmod +x setup.sh
    ```
2. Install CMake (at least version 3.13), and GCC cross compiler
    ```bash
    sudo apt-get update
    sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
    ```
3. Build as follows:
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```
4. If you see `mbot_teleoperation.uf2` in your `/build/` folder, you've finished pre-lab setup.
__________________________________

## Visualize Serial Output

1. Open PowerShell in **administrator** mode and install `usbipd`. Refer to [Microsoft's Official USB Connection](https://learn.microsoft.com/en-us/windows/wsl/connect-usb) for more info.
    ```powershell
    winget install --interactive --exact dorssel.usbipd-win
    ```
2. Follow the [instructions](https://learn.microsoft.com/en-us/windows/wsl/connect-usb#attach-a-usb-device) until Step 4.
    ```powershell
    # In PowerShell:
    # List all USB devices
    usbipd list
    # Share device permission to be attached to WSL
    usbipd bind --busid <BUSID>
    # Attach device to WSL
    usbipd attach --wsl --busid <BUSID>

    # In WSL, see if device attached
    lsusb
    ```
    You should see USB Serial Device **attached** in PowerShell

    ![usbipd](media/usbipd.png)

    You should see Pico listed in WSL when you complete.
    
    ![lsusb](media/lsusb.png)
3. Use `screen`

    ```
    sudo screen /dev/ttyACM0 115200
    ```
    Exit `screen`: To exit the serial monitor in screen, press `Ctrl + A`, then `K` to kill the session, or `Ctrl + A`, then `D` to detach.
4. Or use `minicom`
    ```
    sudo minicom -s
    ```

    - Go to Serial port setup and set the Serial device (e.g., /dev/ttyACM0 or /dev/ttyUSB0).
    - Set the baud rate (e.g., 115200), data bits (8), parity (None), and stop bits (1).
    
    Exit `minicom`: To exit, press `Ctrl + A`, then `Z` for help, and then `X` to quit.
5. You should see serial output from terminal like this. Each line represents the current state of the system including motor positions and command values. Refer to `tests/mbot_teleoperation.c` for more details.

    ![screen](media/screen.gif)

## Plotting (Real-time)
Real-time plotting should be smooth in Linux but may experience delay in WSL due to X forwarding. Python and pip need to be installed. It is recommended to create a virtual environment.
```bash
pip install -r requirements.txt
python python/realtime_plot.py
```

For WSL users, WSL itself does not have a built-in X server, so you need to install a separate X server application on your Windows system to use X forwarding. Consider installing [VcXsrv](https://vcxsrv.com/).
```bash
export DISPLAY=:0
```
