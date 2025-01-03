# About
This project was started in High School as a personal project. I wanted to develop a video game, but no game engine worked exactly the way I wanted it to, which was to use C++ as a scripting language, so I decided to write the game without an engine, just using raylib and C++. This project put my basic knowledge of trigonometry and physics to a test, and I was able to create a platformer with it, albeit with not the greatest physics. The game assets were also self-drawn.


# Manual

| Table of contents |
| ------------- |


---
### Compilation Instructions
## Dependencies
1. A C++ compiler
2. Raylib <br/> 

## To install all dependencies in one command:
Arch Based Linux Distros:
  ```
pacman -S raylib gcc
  ```

## To compile the game
  ```
g++ main.cpp -lraylib -O3 -o Platformer
  ```

## To compile the level editor
  ```
g++ levelDesigner.cpp -lraylib -O3 -o LevelEditor
  ```

## You can also use the makefile
  ```
makefile
  ```
<br/>
If you want to compile this on Windows, Have fun.	
<br/>

---

### Game Controls
| Action | Keypress |
| ------ | -------- |
| Move Right | D |
| Move Left | A |
| Jump | SPACE |
| Open console | / |
| Submit console command | ENTER |
| Pause | ESCAPE |
| Set Checkpoint * | Minus |

\* Checkpoints will be disabled in the future. <br>
\* Controllers are technically supported but all inputs are binary. <br>

---
### Level Editor Controls/Commands
| Action | Keypress/Command/Menu Option |
| ------ | ---------------------------- |
| Move viewport | Middle Mouse Button + Mouse Movement |
| Zoom viewport * | Scroll Wheel Up/Down |
| Place block | Left Click |
| Destroy block | Right Click |
| Choose block | Click the desired block on the left menu |
| Change default rotation | R |
| Save level * | `/saveas <level name>` |
| Load level * | `/load <level name>` |
| Set level background color * | `/set background <r> <g> <b>` |
| Show FPS | `/showfps` |
| Set FPS Cap * | `/set fps <number>` |

\* The editor does not prevent you from setting a negative zoom. Blocks may not be placed in their expected location if in a negative zoom. <br>
\* Full filename is expected. Will be saved into `/levels/` <br>
\* Full filename is expected. Level must be in `/levels/` <br>
\* The values of RGB are 0-255 <br>
\* Framerate is capped to monitor refresh rate by default. The value of number sets the MAX framerate.

---
### Game commands
| Action | Command |
| ------ | ------- |
| Load a level * | `/load <level name>` |
| Toggle FPS counter * | `/showfps` |
| Toggle fullscreen | `/fullscreen` |
| Reset level | `/reset level` |
| Reset save data * | `/reset savedata` |
| Set max framerate * | `/set fps <maxFPS>` |
| Skip currently playing song | `/skip music` |

\* Levels must be in `/levels/` <br>
\* You will also see a graph showing your frametimes <br>
\* Save data is written if you switch levels using portals <br>
\* Framerate is uncapped by default <br>

<br/>
Legal

All files in this repository are licensed under the [GPL-V3](LICENSE) <br/>
All music files on this repository came from [Freepd](https://freepd.com/).
