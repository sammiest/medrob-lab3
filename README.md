# MedRob

## Table of Content

- [Description](#description)
- [Installation](#installation)
- [Code Structure](#code-structure)
- [MBot Classic Usage and Features](#mbot-classic-usage-and-features)
- [MBot OMNI Usage and Features](#mbot-omni-usage-and-features)
- [Generating New Releases](#generating-new-releases)
- [Maintainers](#maintainers)

## Git Installation
1. Check for existing SSH keys: [link](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/checking-for-existing-ssh-keys)

    ```bash
    ls -al ~/.ssh
    ```

    If you don't have a supported public and private key pair, or don't wish to use any that are available, generate a new SSH key. (Continue step 2)

    If you see an existing public and private key pair listed (for example, *id_rsa.pub* and *id_rsa*) that you would like to use to connect to GitHub, you can add the key to the ssh-agent. (Skip to step 3)

2. Generate a new SSH key: [link](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent)

    ```bash
    ssh-keygen -t ed25519 -C "your_email@example.com"
    ```

    You can hit **Enter** to skip changing default ssh file location and setting secure passphrase

3. Add your SSH key to the ssh-agent: [link](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent)

    ```bash
    eval "$(ssh-agent -s)"
    ssh-add ~/.ssh/id_ed25519
    ```

4. Adding a new SSH key to your GitHub account: [link](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/adding-a-new-ssh-key-to-your-github-account)

    ```bash
    cat ~/.ssh/id_ed25519.pub
    # Then select and copy the contents of the id_ed25519.pub file displayed in the terminal to your clipboard
    ```

## Installation

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
