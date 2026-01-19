# Play As Gobo

[![Build Status](https://github.com/almohtadyg1/Play-As-Gobo/actions/workflows/build.yml/badge.svg?branch=main)](https://github.com/almohtadyg1/Play-As-Gobo/actions/workflows/build.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Release](https://img.shields.io/github/v/release/almohtadyg1/Play-As-Gobo)](https://github.com/almohtadyg1/Play-As-Gobo/releases)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-blue.svg)](https://github.com/almohtadyg1/Play-As-Gobo)

A fast-paced 2D action game where you flip the script—**you are the enemy**. Built with modern C++17 and [Raylib](https://www.raylib.com/), Play-As-Gobo offers smooth gameplay mechanics and nostalgic vibes for OG gamers.

## ✨ Features

- 🎮 **Role Reversal Gameplay** - Experience the game from the enemy's perspective
- 🎯 **Smooth Physics & Controls** - Responsive movement and collision detection
- 💥 **Dynamic Explosions** - Advanced particle system with realistic physics
- 🔊 **Immersive Audio** - Sound effects and background music
- 🏆 **Progressive Difficulty** - Enemies scale dynamically as you survive
- 🎨 **Retro-Inspired Graphics** - Clean 2D visuals with modern polish
- 🖥️ **Cross-Platform** - Runs on Windows, Linux, and macOS

## 🎮 Controls

| Action | Key |
|--------|-----|
| Move Left/Right | `←` / `→` Arrow Keys |
| Create Explosion | `Space` |
| Pause/Menu | `Esc` |

## 🚀 Quick Start

### Option 1: Download Pre-built Binary (Recommended)

Download the latest release for your platform:

**[📦 Download Latest Release](https://github.com/almohtadyg1/Play-As-Gobo/releases/latest)**

- **Windows**: Extract `PlayAsGobo-Windows-Release.zip` and run `PlayAsGobo.exe`
- **Linux**: Extract `PlayAsGobo-Linux-Release.tar.gz` and run `./PlayAsGobo`
- **macOS**: Extract `PlayAsGobo-macOS-Release.zip` and open `PlayAsGobo.app`

### Option 2: Build From Source

#### Prerequisites

- **CMake** 3.15 or higher
- **C++17** compatible compiler (GCC 7+, Clang 5+, MSVC 2019+)
- **Git** with submodules support

#### 1️⃣ Clone the Repository

```bash
git clone https://github.com/almohtadyg1/Play-As-Gobo.git
cd PlayAsGobo
```

#### 2️⃣ Install Platform Dependencies

<details>
<summary><b>Windows (MSYS2/MinGW)</b></summary>

```bash
# Install MSYS2 from https://www.msys2.org/
# Then in MSYS2 UCRT64 terminal:
pacman -S mingw-w64-ucrt-x86_64-gcc \
          mingw-w64-ucrt-x86_64-cmake \
          mingw-w64-ucrt-x86_64-ninja
```
</details>

<details>
<summary><b>Windows (Visual Studio)</b></summary>

Install [Visual Studio 2019+](https://visualstudio.microsoft.com/) with:
- Desktop development with C++
- CMake tools for Windows
</details>

<details>
<summary><b>Linux (Ubuntu/Debian)</b></summary>

```bash
sudo apt update
sudo apt install build-essential cmake ninja-build \
                 libx11-dev libxrandr-dev libxi-dev \
                 libxcursor-dev libxinerama-dev \
                 libgl1-mesa-dev libasound2-dev pkg-config
```
</details>

<details>
<summary><b>Linux (Fedora/RHEL)</b></summary>

```bash
sudo dnf install gcc-c++ cmake ninja-build \
                 libX11-devel libXrandr-devel libXi-devel \
                 libXcursor-devel libXinerama-devel \
                 mesa-libGL-devel alsa-lib-devel
```
</details>

<details>
<summary><b>Linux (Arch)</b></summary>

```bash
sudo pacman -S base-devel cmake ninja libx11 libxrandr \
               libxi libxcursor libxinerama mesa alsa-lib
```
</details>

<details>
<summary><b>macOS</b></summary>

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew (if not already installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install CMake
brew install cmake ninja
```
</details>

#### 3️⃣ Build the Project

```bash
# Create build directory
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build (uses all CPU cores)
cmake --build build --config Release --parallel

# Optional: Build with debug symbols
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

#### 4️⃣ Run the Game

```bash
# Windows
./bin/Release/PlayAsGobo.exe

# Linux/macOS
./bin/Release/PlayAsGobo
```

## 🛠️ Advanced Build Options

### Enable AddressSanitizer (Memory Debugging)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON
cmake --build build --config Debug
```

### Cross-Compilation

The project supports cross-platform builds via CMake toolchain files. See [CMakeLists.txt](CMakeLists.txt) for details.

### IDE Support

**Visual Studio Code:**
- Install CMake Tools extension
- Open folder → Select kit → Build

**Visual Studio:**
- Open folder as CMake project
- Select configuration → Build

**CLion:**
- Open as CMake project
- Automatically configured

## 📁 Project Structure

```
PlayAsGobo/
├── .github/
│   └── workflows/        # CI/CD pipelines
├── assets/               # Game assets (textures, sounds, fonts)
│   ├── audio/
│   └── img/
├── include/              # Header files (.hpp)
│   ├── Enemy.hpp
│   ├── Explosion.hpp
│   ├── Game.hpp
│   └── ...
├── src/                  # Source files (.cpp)
│   ├── Enemy.cpp
│   ├── Explosion.cpp
│   ├── Game.cpp
│   ├── main.cpp
│   └── ...
├── libs/
│   └── raylib/           # Raylib library
├── resources/            # Platform-specific resources
│   ├── app.rc
│   ├── icon.ico
│   └── icon.png          # Windows application icon
├── CMakeLists.txt        # Build configuration
├── LICENSE               # MIT License
└── README.md             # This file
```

## 🐛 Troubleshooting

<details>
<summary><b>Build Errors</b></summary>

**"CMake version too old"**
```bash
# Download latest CMake from https://cmake.org/download/
```

**Linker errors on Windows (MinGW)**
```bash
# Ensure you're using UCRT64 environment, not MINGW64
# Reinstall: pacman -S mingw-w64-ucrt-x86_64-toolchain
```
</details>

<details>
<summary><b>Runtime Errors</b></summary>

**"Cannot find assets" or missing textures**
```bash
# Assets should be in the same directory as the executable
# Ensure bin/Release/assets/ exists and contains game files
```

**Segmentation fault on Linux**
```bash
# Check graphics drivers are up to date
# Install: sudo apt install mesa-utils
# Test: glxinfo | grep "OpenGL version"
```

**Permission denied (Linux/macOS)**
```bash
chmod +x bin/Release/PlayAsGobo
```
</details>

<details>
<summary><b>Performance Issues</b></summary>

**Low FPS**
- Update graphics drivers
- Close background applications
- Try Release build instead of Debug

**High CPU usage**
- This is normal for Debug builds
- Use Release builds for production
</details>

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request. For major changes:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- [Raylib](https://www.raylib.com/) - Amazing game development framework
- [CMake](https://cmake.org/) - Cross-platform build system
- All contributors and playtesters

## 📧 Contact

**Almohtady Bellah**
- Portfolio: [almohtadyg1.pythonanywhere.com](https://almohtadyg1.pythonanywhere.com/profile)
- GitHub: [@almohtadyg1](https://github.com/almohtadyg1)

---

<p align="center">
  <b>⭐ If you enjoy the game, please consider starring this repository! ⭐</b>
</p>

<p align="center">
  Made with ❤️ by <a href="https://almohtadyg1.pythonanywhere.com/profile">Almohtady Bellah</a>
</p>
