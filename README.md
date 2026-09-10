# Saw Chase

A small 2D game written in **C++** using **SDL2** and **SDL2_image**.

## Gameplay

You play as a character who must collect coins while being chased by a spinning circular saw.

* Collect coins to survive.
* Avoid the spinning saw.
* If the saw catches you, you lose.
* Simple procedural sound effects are generated in code.

## Features

* Written in modern C++
* SDL2 rendering
* SDL2_image for textures
* Simple collision detection
* Enemy AI that follows the player
* Procedural audio (no external sound files)
* Cross-platform (Linux & Windows)

## Controls

| Key | Action     |
| --- | ---------- |
| W   | Move Up    |
| A   | Move Left  |
| S   | Move Down  |
| D   | Move Right |

## Project Structure

```text
src/
├── Collision.h
├── main.cpp
├── pt_audio.hpp
└── tmps/
    ├── bak.png
    └── imgs.png
```

## Build

### Linux

```bash
g++ main.cpp -o game $(sdl2-config --cflags --libs) -lSDL2_image
```

### Windows (MinGW-w64)

Cross-compiled on Linux using **MinGW-w64**.

## Dependencies

* SDL2
* SDL2_image
* C++17 compatible compiler

## how to run

in linux:


```bash
git clone https://github.com/pin-toto/saw_chase.git
cd saw_chase
g++ main.cpp -O3 -o SawChase $(sdl2-config --cflags --libs) -lSDL2_image
./SawChase
```
in win:
```cmd
git clone https://github.com/pin-toto/saw_chase.git
cd saw_chase
g++ main.cpp -o SawChase.exe -lSDL2main -lSDL2 -lSDL2_image
.\SawChase.exe

```

---
## License

This project is open source and available under the MIT License.



Made with ❤️ using C++ and SDL2.
