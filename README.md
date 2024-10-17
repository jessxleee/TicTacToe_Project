# TicTacToe_Project
**Installing GTK 3.0**
1. Download MSYS2 Installer from https://www.msys2.org/
2. Install MSYS2 and open the MSYS2 Shell
3. Update the package database
```bash
    pacman -Syu
```
4. Install GTK with :
```bash 
pacman -S mingw-w64-x86_64-gtk3'
```

**Compiling GTK Application**
1. To ensure that your compilier recognises GTK 3.0, open up you c_cpp_properties.json and include the below into your includePath:
```bash
  "includePath": [
               "${workspaceFolder}/**",
                "C:/msys64/mingw64/include/gtk-3.0",
                "C:/msys64/mingw64/include/glib-2.0",
                "C:/msys64/mingw64/lib/glib-2.0/include",
                "C:/msys64/mingw64/include/gdk-pixbuf-2.0",
                "C:/msys64/mingw64/include/pango-1.0",
                "C:/msys64/mingw64/include/atk-1.0",
                "C:/msys64/mingw64/include/cairo",
                "C:/msys64/mingw64/include/SDL2",
                "C:/msys64/mingw64/include/harfbuzz",
                "C:/msys64/mingw64/include"
            ],

```

2. Open up MSYS2 MINGW64 shell (Note: Different from MSYS2 Shell)
3. Change to your Respository Directory 
   Example:
   ```bash
   cd: Users/user/TicTacToe_Project
   ```
4. Compile and run the program:
```bash
 gcc index -o index.c $(pkg-config --cflags --libs gtk+-3.0)
 ./index.c
```
5. if gcc command not found, install GCC for the 64-bit environment:
```bash
pacman -S mingw-w64-x86_64-gcc
```