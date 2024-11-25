# TicTacToe_Project

**Installing GTK 4.0 and other dependencies**

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
pacman -S --noconfirm mingw-w64-x86_64-SDL2_mixer mingw-w64-x86_64-python mingw-w64-x86_64-python-scikit-learn mingw-w64-x86_64-python-matplotlib
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
2.  Install Dependencies (using Homebrew)
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

7. Install SDL2 and Mixer
Download SDL2 by picking the .dmg one from the release list:
https://github.com/libsdl-org/SDL/releases

Additionally, download the SDL2_image library (also a .dmg from its release list:
https://github.com/libsdl-org/SDL_image/releases

Execute the SDL2 file (will be named something similar to SDL2-2.30.4.dmg) and a window will open with a few files and an SDL2.framework folder.

Open Finder, click Go then Go to folder... and type /Library/Frameworks which will open the contents of that folder. Drag SDL2.framework to /Library/Frameworks and repeat the steps for the SDL2_image file.

There should be two additional folders in /Library/Frameworks, SDL2.framework and SDL2_image.framework.

8. Install Python (using Homebrew)
```bash
brew install python
```

9. Install Python dependencies in virtual environment
```
cd TicTacToe_Project
python3 -m venv venv
source venv/bin/activate
pip install --upgrade pip
pip install matplotlib scikit-learn
deactivate
```
