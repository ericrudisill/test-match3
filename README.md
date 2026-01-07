# Match-3 Game

A cross-platform Match-3 puzzle game built with C++ and SDL3, supporting Desktop (Linux, macOS, Windows), Android, and iOS.

## Features

- Classic Match-3 gameplay mechanics
- Smooth animations and visual feedback
- Touch and mouse input support
- Cross-platform builds using CMake
- Cascading matches with gravity
- Score tracking

## Prerequisites

### All Platforms
- CMake 3.21 or higher
- C++17 compatible compiler
- **SDL3** (Simple DirectMedia Layer 3)
- **SDL3_ttf** (TrueType font rendering for SDL3)
- **SDL3_image** (Image loading for SDL3)

> **Important:** SDL3 and its companion libraries are still in development and not yet available in most package managers. You'll need to build them from source.
> **See [docs/SDL3-Installation.md](docs/SDL3-Installation.md) for detailed installation instructions.**

### Quick SDL3 Installation

> **Note:** Make sure you have CMake installed first (see Additional Desktop Dependencies below)

#### Linux (User-Local, No Sudo Required)
```bash
cd /tmp
```
```bash
git clone https://github.com/libsdl-org/SDL.git SDL3 --depth 1 --branch main
```
```bash
cd SDL3 && mkdir build && cd build
```
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/.local
```
```bash
cmake --build . -j$(nproc)
```
```bash
cmake --install .
```

#### macOS (User-Local, No Sudo Required)
```bash
cd /tmp
```
```bash
git clone https://github.com/libsdl-org/SDL.git SDL3 --depth 1 --branch main
```
```bash
cd SDL3 && mkdir build && cd build
```
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/.local
```
```bash
cmake --build . -j$(sysctl -n hw.ncpu)
```
```bash
cmake --install .
```

#### Windows
See [docs/SDL3-Installation.md](docs/SDL3-Installation.md) for Windows-specific instructions.

### SDL3_ttf Installation

SDL3_ttf provides TrueType font rendering support.

#### macOS

First, install Freetype and Harfbuzz via Homebrew:
```bash
brew install freetype harfbuzz
```

Then build SDL3_ttf:
```bash
cd /tmp
```
```bash
git clone https://github.com/libsdl-org/SDL_ttf.git SDL3_ttf --depth 1 --branch main
```
```bash
cd SDL3_ttf && mkdir build && cd build
```
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/.local
```
```bash
cmake --build . -j$(sysctl -n hw.ncpu)
```
```bash
cmake --install .
```

#### Linux

```bash
cd /tmp
```
```bash
git clone https://github.com/libsdl-org/SDL_ttf.git SDL3_ttf --depth 1 --branch main
```
```bash
cd SDL3_ttf && mkdir build && cd build
```
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/.local
```
```bash
cmake --build . -j$(nproc)
```
```bash
cmake --install .
```

### SDL3_image Installation

SDL3_image provides image loading support for various formats.

#### macOS

```bash
cd /tmp
```
```bash
git clone https://github.com/libsdl-org/SDL_image.git SDL3_image --depth 1 --branch main
```
```bash
cd SDL3_image && mkdir build && cd build
```
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/.local
```
```bash
cmake --build . -j$(sysctl -n hw.ncpu)
```
```bash
cmake --install .
```

#### Linux

```bash
cd /tmp
```
```bash
git clone https://github.com/libsdl-org/SDL_image.git SDL3_image --depth 1 --branch main
```
```bash
cd SDL3_image && mkdir build && cd build
```
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/.local
```
```bash
cmake --build . -j$(nproc)
```
```bash
cmake --install .
```

### Additional Desktop Dependencies

#### Linux
```bash
# Ubuntu/Debian
sudo apt-get install cmake build-essential pkg-config \
    libx11-dev libxext-dev libwayland-dev libxkbcommon-dev \
    libegl1-mesa-dev libgles2-mesa-dev
```

#### macOS

**Step 1:** Install Xcode Command Line Tools:
```bash
xcode-select --install
```

**Step 2:** If you don't have Homebrew installed, install it (requires password):
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

**Step 3:** Install CMake:
```bash
brew install cmake
```

#### Windows
1. Install [Visual Studio 2019 or later](https://visualstudio.microsoft.com/) with C++ support
2. Install [CMake](https://cmake.org/download/)

### Android

1. Install [Android Studio](https://developer.android.com/studio)
2. Install Android NDK through Android Studio SDK Manager
3. Install CMake through Android Studio SDK Manager
4. Download SDL3 for Android from [libsdl.org](https://www.libsdl.org/)

### iOS

1. macOS with Xcode 12 or later
2. iOS SDK
3. SDL3 for iOS (can be built from source or use pre-built framework)

## Building

### Desktop (Linux/macOS)

```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
cmake --build .

# Run the game
./Match3Game
```

### Desktop (Windows)

```bash
# Create build directory
mkdir build
cd build

# Configure (adjust generator as needed)
cmake .. -G "Visual Studio 17 2022"

# Build
cmake --build . --config Release

# Run the game
Release\Match3Game.exe
```

### Android

#### Method 1: Using Android Studio (Recommended)

1. Create an Android project structure:
```bash
mkdir -p android-project/app/src/main
cp -r android/* android-project/app/src/main/
```

2. Create `android-project/app/build.gradle`:
```gradle
plugins {
    id 'com.android.application'
}

android {
    namespace 'com.match3game.app'
    compileSdk 34

    defaultConfig {
        applicationId "com.match3game.app"
        minSdk 21
        targetSdk 34
        versionCode 1
        versionName "1.0"

        externalNativeBuild {
            cmake {
                arguments "-DANDROID_STL=c++_shared"
                cppFlags "-std=c++17"
            }
        }
        ndk {
            abiFilters 'armeabi-v7a', 'arm64-v8a', 'x86', 'x86_64'
        }
    }

    buildTypes {
        release {
            minifyEnabled false
        }
    }

    externalNativeBuild {
        cmake {
            path file('../../CMakeLists.txt')
            version '3.22.1'
        }
    }
}

dependencies {
    implementation fileTree(dir: 'libs', include: ['*.jar'])
    // Add SDL3 AAR dependency here when available
}
```

3. Open the `android-project` directory in Android Studio
4. Build and run on device or emulator

#### Method 2: Command Line with Gradle

```bash
cd android-project
./gradlew assembleDebug

# Install on connected device
adb install app/build/outputs/apk/debug/app-debug.apk
```

### iOS

1. Configure for iOS:
```bash
mkdir build-ios
cd build-ios

cmake .. \
    -G Xcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="YOUR_TEAM_ID"
```

2. Open the generated Xcode project:
```bash
open Match3Game.xcodeproj
```

3. In Xcode:
   - Select your development team
   - Choose a connected device or simulator
   - Build and run (⌘R)

## How to Play

1. **Objective**: Match 3 or more gems of the same color in a row or column
2. **Controls**:
   - **Desktop**: Click and drag gems to swap them with adjacent gems
   - **Mobile**: Touch and swipe gems to swap them
3. **Scoring**: Each matched gem awards points
4. **Strategy**: Create cascading matches for higher scores

## Game Controls

- **Desktop**:
  - Mouse click and drag to swap gems
  - ESC key to quit

- **Mobile**:
  - Touch and swipe to swap gems
  - Use device back button to exit

## Project Structure

```
.
├── CMakeLists.txt           # Main CMake configuration
├── src/                     # Source files
│   ├── main.cpp            # Entry point
│   ├── Game.cpp/h          # Main game loop and state management
│   ├── Grid.cpp/h          # Grid logic and match detection
│   ├── Gem.cpp/h           # Gem entity and animations
│   ├── Renderer.cpp/h      # Rendering system
│   ├── InputHandler.cpp/h  # Input handling for all platforms
│   ├── BoardTypes.h        # Pure data types (no SDL dependency)
│   └── BoardLogic.cpp/h    # Testable game logic
├── tests/                   # Unit tests
│   ├── BoardLogicTests.cpp # Game logic tests
│   └── TestHelpers.h       # Test utilities
├── docs/                    # Documentation
│   └── SDL3-Installation.md # Detailed SDL3 build instructions
├── android/                 # Android-specific files
│   ├── android.cmake       # Android CMake configuration
│   └── AndroidManifest.xml # Android manifest
├── ios/                     # iOS-specific files
│   ├── ios.cmake           # iOS CMake configuration
│   └── Info.plist          # iOS app configuration
└── README.md               # This file
```

## Customization

### Changing Grid Size

Edit `Grid.h` and modify:
```cpp
static const int ROWS = 8;  // Change to desired rows
static const int COLS = 8;  // Change to desired columns
```

### Adding More Gem Types

Edit `Gem.h` to add more gem types to the `GemType` enum, then update `Renderer.cpp` to add corresponding colors in `getGemColor()`.

### Adjusting Animation Speed

Edit `Gem.cpp` and modify the `ANIMATION_SPEED` constant in the `update()` method.

## Troubleshooting

### SDL3 Not Found (Desktop)

SDL3 is not available in most package managers yet. You need to build it from source:

1. **Quick fix**: Follow the Quick SDL3 Installation instructions above
2. **Detailed instructions**: See [docs/SDL3-Installation.md](docs/SDL3-Installation.md)
3. **If you installed to `~/.local`**: The project is already configured to find it there
4. **Custom install location**: Set `CMAKE_PREFIX_PATH` when configuring:
   ```bash
   cmake .. -DCMAKE_PREFIX_PATH=/path/to/sdl3/install
   ```

### Library Not Found at Runtime (Linux)

If you get "error while loading shared libraries: libSDL3.so" when running the game:

```bash
# Add to ~/.bashrc or ~/.zshrc
export LD_LIBRARY_PATH=$HOME/.local/lib:$LD_LIBRARY_PATH

# Reload your shell
source ~/.bashrc
```

### SDL3 Build Failed

If SDL3 fails to build, ensure you have all dependencies:

```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake git pkg-config \
    libx11-dev libxext-dev libwayland-dev libxkbcommon-dev \
    libegl1-mesa-dev libgles2-mesa-dev libgl1-mesa-dev \
    libasound2-dev libpulse-dev libdbus-1-dev libudev-dev
```

See [docs/SDL3-Installation.md](docs/SDL3-Installation.md) for platform-specific dependency lists.

### Android Build Issues

- Ensure NDK and CMake are installed via Android Studio SDK Manager
- Check that `ANDROID_NDK_HOME` environment variable is set
- Verify that SDL3 for Android is properly integrated

### iOS Code Signing Issues

- Set your development team in Xcode project settings
- Ensure provisioning profiles are properly configured
- Update `XCODE_ATTRIBUTE_DEVELOPMENT_TEAM` in `ios/ios.cmake`

## License

This project is provided as-is for educational purposes.

## Dependencies

- [SDL3](https://www.libsdl.org/) - Simple DirectMedia Layer for cross-platform multimedia

## Testing

The game logic is separated into a testable layer that runs without SDL or graphics.

### Running Tests

```bash
# Configure with tests enabled
cmake -B build -DBUILD_TESTS=ON

# Build
cmake --build build

# Run tests
./build/Match3Tests

# Or use CTest
ctest --test-dir build --output-on-failure
```

### Test Coverage

The test suite covers:
- Match detection (horizontal, vertical, L-shaped, T-shaped)
- Gravity and gem falling
- Swap validation
- Valid moves detection
- Cascade sequences
- Edge cases

### Writing Tests

Tests use the Catch2 framework. Use the helper functions in `tests/TestHelpers.h`:

```cpp
#include "BoardLogic.h"
#include "TestHelpers.h"

TEST_CASE("My test", "[tag]") {
    BoardLogic logic;

    // Create a board with no matches
    auto state = noMatchBoard();

    // Add specific gems for testing
    state.at(0, 0) = GemType::RED;
    state.at(0, 1) = GemType::RED;
    state.at(0, 2) = GemType::RED;

    auto result = logic.checkMatches(state);
    CHECK(result.matchedPositions.size() == 3);
}
```

## Future Enhancements

- Sound effects and music
- Power-ups and special gems
- Multiple game modes
- High score persistence
- Better visual effects and particle systems
- Tutorial mode
- Achievements system
