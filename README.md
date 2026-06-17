# 🎱 2D Physics Engine

A real-time 2D physics simulation built with **C++** and **SFML 3.0**, featuring gravity, elastic collisions, and interactive ball manipulation — a small but genuine physics engine built from first principles.

![Physics Engine Preview](https://raw.githubusercontent.com/snehal-thombare08/physics-engine-cpp/main/PASTE_YOUR_SCREENSHOT_FILENAME_HERE.png)

## ✨ Features

- **Realistic gravity simulation** with adjustable direction (up, down, left, right)
- **Elastic ball-to-ball collisions** with proper momentum and mass-weighted positional correction
- **Wall boundary collisions** with energy loss on bounce (restitution)
- **Interactive ball spawning** — click anywhere to add a new ball
- **Grab and throw mechanics** — drag a ball and release to launch it with real velocity based on mouse motion
- **Speed-based color gradient** — balls shift from calm blue to hot red/orange as their speed increases
- **Multi-substep collision resolution** for stability even with 100+ balls on screen
- **Air friction/drag** for natural energy decay over time
## screenshot
https://raw.githubusercontent.com/snehal-thombare08/physics-engine-cpp/main/Screenshot%202026-06-17%20125610.png

## 🎮 Controls

| Key / Mouse           | Action                                  |
|-------------------------|-------------------------------------------|
| Left Mouse Button       | Spawn a new ball at cursor               |
| Right Mouse Button (drag) | Grab a ball, drag it, release to throw |
| G                        | Toggle gravity on/off                    |
| Arrow Keys                | Change gravity direction                |
| C                         | Clear all balls                          |
| Esc                       | Quit                                      |

## 🧠 How It Works

Each ball is a rigid circle with position, velocity, radius, and mass (derived from radius squared, so bigger balls are heavier). Every frame:

1. **Integration** — gravity and friction are applied to velocity, then position is updated
2. **Wall collisions** — balls bounce off screen edges, losing some energy on each bounce (restitution)
3. **Ball-to-ball collisions** — for every pair of overlapping balls, the engine separates them proportionally to their mass, then applies an impulse-based elastic collision response along the contact normal

Collision resolution runs across multiple sub-steps per frame, which keeps large stacks of balls stable instead of jittering or sinking into each other — the same general approach used in simple physics engines for games.

## 🛠️ Built With

- **C++17**
- **SFML 3.0** — windowing, rendering, and input
- **CMake** — build configuration
- **MinGW** — compiler toolchain (Windows)

## 🚀 Building from Source

### Prerequisites
- CMake 3.16+
- MinGW-w64 (or any C++17 compiler)
- SFML 3.0 (via vcpkg recommended)

### Build Steps

```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg.cmake -G "MinGW Makefiles"
mingw32-make
./PhysicsEngine.exe
```

## 📦 Download

Prebuilt Windows binary available under [Releases](../../releases) — just download, extract, and run `PhysicsEngine.exe`.

## 📄 License

This project is open source and available for learning and experimentation.

---

⭐ If you found this interesting, consider starring the repo!
