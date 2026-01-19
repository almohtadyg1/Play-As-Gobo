# Play As Gobo

A C++ lightweight game built with [Raylib](https://www.raylib.com/).

## Features

- You are the enemy themed game
- Smooth controls and animations
- Play as Gobo in a 2D environment
- Simple and fun gameplay for OG gamers

## Getting Started

### PlayAsGobo - Build Instructions

##### 1. Get the Code
```bash
git clone --recursive https://github.com/almohtadyg1/PlayAsGobo.git
cd PlayAsGobo
```

##### 2. Install Dependencies (Skip if you have cmake and a compiler!)

**Windows:**
- Install Visual Studio 2019+ with C++ support, OR
- Install MSYS2 and run: `pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake`

**Linux (Ubuntu/Debian):**
```bash
sudo apt install build-essential cmake libx11-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev libgl1-mesa-dev libasound2-dev
```

**Linux (Fedora):**
```bash
sudo dnf install gcc-c++ cmake libX11-devel libXrandr-devel libXi-devel libXcursor-devel libXinerama-devel mesa-libGL-devel alsa-lib-devel
```

**macOS:**
```bash
xcode-select --install
brew install cmake
```

##### 3. Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

##### 4. Run
```bash
# The game will be in bin/Release/
./bin/Release/PlayAsGobo
```

#### 🔧 Troubleshooting

**"Address sanitizer error" on Windows MSYS2:**
```bash
# Use this instead:
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O2"
```

**"Raylib not found":**
```bash
# Make sure you ran:
git submodule update --init --recursive
```

**"Missing libraries" on Linux:**
```bash
# Install the development packages listed above for your distro
```

**Permission denied on Linux/macOS:**
```bash
chmod +x bin/Release/PlayAsGobo
```

---

### How to run the game (Windows) 🎮
1. You can run game by simply extracting and launching "(Latest Version)[https://github.com/almohtadyg1/Play-As-Gobo/releases] of the game."

or

2. [Build It Your Self](#build-instructions)

---

### Project Structure
```
.
├── assets/
├── resources/
├── include/
├── src/
├── CMakeLists.txt
└── README.md
```

## Controls

- **Arrow Keys**: Move Gobo
- **Space**: Jump
- **Esc**: Back

## License

MIT License

---

Made By [Almohtady Bellah](https://almohtadyg1.pythonanywhere.com/profile)