
# Chase of The Lost

A 2D top-down game built with **SDL2** where players fight against waves of enemies, aiming to survive as long as possible while achieving a high score.

## Game Overview

In this action-packed survival game, you control a player character navigating through a battlefield filled with hostile enemies. Survive as long as possible, eliminate enemies, and achieve the highest score!

## Gameplay Features

- **Player Movement**: Move freely across the game area using `W`, `A`, `S`, `D` keys.
- **Revolving Weapon**: A red dot orbits the player, eliminating enemies on contact.
- **Enemy AI**: Enemies actively chase the player, increasing difficulty over time.
- **Health System**: The player starts with a limited health pool and loses HP on enemy collision.
- **Bullet Shooting**: Once the player scores 30 points, they gain the ability to shoot bullets at enemies.
- **Fullscreen Toggle**: Switch between fullscreen and windowed mode using the `F` key.
- **High Scores**: The game automatically saves and displays the highest score.

## Controls

| Action             | Key/Control         |
|--------------------|---------------------|
| Move Up           | `W`                |
| Move Down         | `S`                |
| Move Left         | `A`                |
| Move Right        | `D`                |
| Shoot Bullets     | Left Mouse Button  |
| Toggle Fullscreen | `F`                |

## How to Play

1. Navigate the player character using the movement keys.
2. Avoid enemy collisions to preserve your health.
3. Use the revolving red dot to eliminate enemies. 
4. After achieving a score of 30, use the left mouse button to shoot bullets at enemies.
5. Survive as long as possible to set a new high score.

## Technical Details

### Built With:
- **Language**: C
- **Library**: SDL2

### Key Functionalities:
- **Entity Management**: Modular handling of player and enemy entities with dynamic spawning and collision detection.
- **Scaling Support**: Responsive scaling for different screen resolutions.
- **High Score Persistence**: High scores are saved locally for replayability.

## Installation and Usage
- Clone the repository:

    ```bash
    git clone https://github.com/Fayz-Rahman/scg.git
    cd scg
    ```
    or download zipfile of the code:
    https://github.com/Fayz-Rahman/scg/archive/refs/heads/main.zip

- Go to dist folder and run COTL.exe file.

## Build Instructions
### Prerequisites
- Ensure you have a C compiler installed (e.g, GCC).
- SDL2 development libraries must be set up:
  - **Windows**: Download the SDL2 `.dll` and development files from [SDL's official website](https://www.libsdl.org/download-2.0.php).

### Setting Up the Project
1. Follow the steps from [**Installation and Usage**](#installation-and-usage) section.

2. Move to the src directory.

3. Build the project using your preferred compiler. For example:

    ```bash
    gcc COTL.c -o ../dist/COTL.exe -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -mwindows
    ```

## Screenshots

*no ss yet*

## Roadmap

- Add power-ups for the player.
- Introduce new enemy types with unique behaviors.
- Implement a multiplayer mode.

## Acknowledgments

- **SDL2**: For providing a robust multimedia library.
- **PixelFrog**: For the "Tiny Swords" asset pack, available under the CC0 license on [itch.io](https://pixelfrog-assets.itch.io/tiny-swords).
- **Community Tutorials**: For helpful guidance and inspiration.
