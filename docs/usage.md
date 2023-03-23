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

## To actually compile it
  ```
g++ main.cpp -lraylib -O3
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
| Choose block | Click the desired block on the left menu * |
| Show laser beams * | `/showlasers` |
| Save level * | `/saveas <level name>` |
| Load level * | `/load <level name>` |
| Set level background color * | `/set background <r> <g> <b>` |

\* The editor does not prevent you from setting a negative zoom. Blocks may not be placed in their expected location if in a negative zoom. <br>
\* Laser beams do not perform collision checking in the editor, meaning they will pass through blocks. This is not true in the actual game. <br>
\* Full filename is expected. Will be saved into `/levels/` <br>
\* Full filename is expected. Level must be in `/levels/` <br>
\* The values of RGB are 0-255

---
### Game commands
| Action | Command |
| ------ | ------- |
| Load a level * | `/load <level name>` |
| Toggle FPS counter * | `/showfps` |
| Randomly generate a level * | `/generate <width> <height>` |
| Toggle fullscreen | `/fullscreen` |

\* Levels must be in `/levels/` <br>
\* You will also see a graph showing your frametimes <br>
\* The generated level will have a random name as well