## Github SSH Setup
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
    # Select and copy the contents of the id_ed25519.pub file displayed in the terminal to your clipboard
    # Then follow above link
    ```

5. Check SSH setup:
    ```bash
    ssh -T git@github.com
    # You should see:
    # Hi username! You've successfully authenticated, but GitHub does not provide shell access.
    ```