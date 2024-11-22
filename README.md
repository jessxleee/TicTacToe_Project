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

