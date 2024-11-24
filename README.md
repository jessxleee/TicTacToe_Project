# TicTacToe_Project

**Installing GTK 4.0**
For windows:
1. Download MSYS2 Installer from https://www.msys2.org/
2. Install MSYS2 and open the MSYS2 Shell
3. Update the package database
```bash
    pacman -Syu
```
4. Install GTK4 with :
```bash 
pacman -S mingw-w64-x86_64-gtk4
```
Install other dependencies used in the project:
```bash 
pacman -S mingw-w64-x86_64-SDL2_mixer
```

5. Open up MSYS2 MINGW64 shell (Note: Different from MSYS2 Shell)
6. Change to your Respository Directory 
   Example:
   ```bash
   cd: Users/user/TicTacToe_Project
   ```
7. Compile the program.
```bash
make clean
make
```

8. Run the program.
```bash
make run
```

For MacOS:
1. Install Homebrew if it's not already installed:
```bash
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```
2.  Install Dependencies
```bash
    brew install gtk4
``` 
4. Export the required variables in your .zshrc or .bashrc file:
```bash
    export PATH="/usr/local/bin:$PATH"
    export XDG_DATA_DIRS="/usr/local/share:$XDG_DATA_DIRS"
    export GTK_PATH="/usr/local/lib/gtk-4.0"
```
5. Reload the shell configuration:
```bash
    source ~/.zshrc  # Or `source ~/.bashrc`
```
6. Verify Installation
```bash
    gtk4-demo
```


