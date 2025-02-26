# MedRob

## Table of Content

- [Description](#description)
- [Installation](#installation)
- [Code Structure](#code-structure)
- [MBot Classic Usage and Features](#mbot-classic-usage-and-features)
- [MBot OMNI Usage and Features](#mbot-omni-usage-and-features)
- [Generating New Releases](#generating-new-releases)
- [Maintainers](#maintainers)



## Installation
sudo apt install x11-apps

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
4. Drag the `mbot_encoder_test.uf2` into the mounted Pico

## To Visualize Encoder Output
1. Use `screen`

    ```
    screen /dev/ttyACM0 115200
    ```
    Exit screen: To exit the serial monitor in screen, press `Ctrl + A`, then `K` to kill the session, or `Ctrl + A`, then `D` to detach.
2. Or use `minicom`
    ```
    sudo minicom -s
    ```

    - Go to Serial port setup and set the Serial device (e.g., /dev/ttyACM0 or /dev/ttyUSB0).
    - Set the baud rate (e.g., 115200), data bits (8), parity (None), and stop bits (1).
    
    Exit minicom: To exit, press `Ctrl + A`, then `Z` for help, and then `X` to quit.
