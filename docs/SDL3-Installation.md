# Building and Installing SDL3

SDL3 is the latest version of the Simple DirectMedia Layer library. Since it's still in development, it's not yet available in most distribution package managers. This guide shows you how to build and install SDL3 from source.

## Prerequisites

### Linux (Debian/Ubuntu)
```bash
sudo apt-get update
sudo apt-get install build-essential cmake git pkg-config \
    libx11-dev libxext-dev libwayland-dev libxkbcommon-dev \
    libegl1-mesa-dev libgles2-mesa-dev libgl1-mesa-dev \
    libasound2-dev libpulse-dev libdbus-1-dev libudev-dev
```

### macOS
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install CMake (if not already installed)
brew install cmake
```

### Windows
1. Install [Visual Studio 2019 or later](https://visualstudio.microsoft.com/) with C++ support
2. Install [CMake](https://cmake.org/download/)
3. Install [Git for Windows](https://git-scm.com/download/win)

## Building SDL3

### Step 1: Clone the SDL3 Repository

```bash
# Clone to a temporary location
cd /tmp
git clone https://github.com/libsdl-org/SDL.git SDL3 --depth 1 --branch main
cd SDL3
```

**Note:** The `main` branch contains SDL3. The `--depth 1` flag reduces download size by getting only the latest commit.

### Step 2: Create Build Directory

```bash
mkdir build
cd build
```

### Step 3: Configure the Build

#### Option A: System-Wide Installation (requires sudo)
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

This will install to `/usr/local` by default.

#### Option B: User-Local Installation (recommended, no sudo)
```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$HOME/.local
```

This installs SDL3 to `~/.local`, which doesn't require administrator privileges.

#### Windows-Specific Configuration
```bash
# For Visual Studio 2022
cmake .. -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release

# For user-local installation on Windows
cmake .. -G "Visual Studio 17 2022" -DCMAKE_INSTALL_PREFIX=%USERPROFILE%\.local
```

### Step 4: Build SDL3

```bash
# Linux/macOS - use all available CPU cores
cmake --build . -j$(nproc)

# Windows
cmake --build . --config Release
```

This will take several minutes depending on your system.

### Step 5: Install SDL3

#### System-Wide Installation
```bash
# Linux/macOS
sudo cmake --install .

# Windows (run as Administrator)
cmake --install .
```

#### User-Local Installation
```bash
# Linux/macOS
cmake --install .

# Windows
cmake --install .
```

### Step 6: Verify Installation

Create a test file `test_sdl3.cpp`:
```cpp
#include <SDL3/SDL.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }
    printf("SDL3 initialized successfully!\n");
    SDL_Quit();
    return 0;
}
```

Compile and run:
```bash
# If installed to ~/.local
g++ test_sdl3.cpp -I$HOME/.local/include/SDL3 -L$HOME/.local/lib -lSDL3 -o test_sdl3
./test_sdl3

# If installed system-wide
g++ test_sdl3.cpp -lSDL3 -o test_sdl3
./test_sdl3
```

## Troubleshooting

### Library Not Found (Linux)

If you get errors about SDL3 not being found when running your program:

```bash
# Add to ~/.bashrc or ~/.zshrc
export LD_LIBRARY_PATH=$HOME/.local/lib:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=$HOME/.local/lib/pkgconfig:$PKG_CONFIG_PATH

# Reload your shell
source ~/.bashrc  # or source ~/.zshrc
```

### CMake Can't Find SDL3

Make sure your CMakeLists.txt includes the local install path:
```cmake
list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/.local")
find_package(SDL3 REQUIRED)
```

### Windows DLL Issues

If you get "DLL not found" errors on Windows:
1. Copy `SDL3.dll` from the build directory to your executable directory
2. Or add the SDL3 `bin` directory to your PATH environment variable

### Missing Dependencies (Linux)

If the build fails due to missing dependencies, install the development packages:

**Wayland support:**
```bash
sudo apt-get install libwayland-dev wayland-protocols libxkbcommon-dev
```

**Audio support:**
```bash
sudo apt-get install libasound2-dev libpulse-dev libjack-dev
```

**Video support:**
```bash
sudo apt-get install libx11-dev libxext-dev libxrandr-dev libxcursor-dev \
    libxi-dev libxinerama-dev libxxf86vm-dev libxss-dev
```

## Updating SDL3

To update to the latest version:

```bash
cd /tmp/SDL3
git pull
cd build
cmake --build . -j$(nproc)
cmake --install .  # Add sudo if system-wide
```

## Uninstalling SDL3

### User-Local Installation
```bash
rm -rf ~/.local/include/SDL3
rm -rf ~/.local/lib/libSDL3*
rm -rf ~/.local/lib/cmake/SDL3
rm -f ~/.local/lib/pkgconfig/sdl3.pc
```

### System-Wide Installation
```bash
sudo rm -rf /usr/local/include/SDL3
sudo rm -rf /usr/local/lib/libSDL3*
sudo rm -rf /usr/local/lib/cmake/SDL3
sudo rm -f /usr/local/lib/pkgconfig/sdl3.pc
```

## Additional Resources

- [SDL3 GitHub Repository](https://github.com/libsdl-org/SDL)
- [SDL3 Migration Guide](https://github.com/libsdl-org/SDL/blob/main/docs/README-migration.md)
- [SDL Wiki](https://wiki.libsdl.org/)
- [SDL Discourse Forum](https://discourse.libsdl.org/)

## Platform-Specific Notes

### Android
SDL3 for Android requires the Android NDK and is typically integrated as part of your Android Studio project. See the [android/README.md](../android/README.md) for details.

### iOS
SDL3 for iOS requires Xcode and can be built as a framework. See the [ios/README.md](../ios/README.md) for details.

### Web (Emscripten)
SDL3 can be built with Emscripten for web deployment:
```bash
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release
emmake make
```
