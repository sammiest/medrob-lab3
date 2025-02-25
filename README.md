# MedRob

## Installation
1. After cloning the repository, run the setup script:

    ```bash
    cd ~/mbot_firmware
    ./setup.sh
    ```
    - which will install dependencies (requires sudo password) and initialize the submodules.

    If setup.sh is not executable by default, run the following to enable it:

    ```bash
    sudo chmod +x setup.sh
    ```
2. Build as follows:
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```
3. Drag the `mbot_encoder_test.uf2` into the mounted Pico

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
