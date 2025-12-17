# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Ms. Pacman implementation in C using the Raylib game framework. This is a reimplementation of the classic arcade game, started after an initial Go version (Ms. Packer Fan) was discontinued. The project emphasizes faithful reproduction of original Pac-Man behavior, including historical bugs like the ghost overflow bug when Pac-Man faces north.

## Build System

Built with CMake (C17 standard). Raylib 5.5 is automatically fetched if not found locally.

### Build Commands

```bash
# Standard build
cmake -B build
cd build
make

# Run the game
./pacman
```

The build automatically copies the `assets/` directory to the build output.

## Asset Requirements

The game requires a 648x1488 PNG spritesheet named `assets/game.png` (not included for copyright reasons). The image contains all sprites and maze layouts stacked vertically. A `font.png` file may also be needed. See `assets/README.md`.

## Architecture

### Core Components

The architecture uses a global `world_t` struct (defined in `main.c`) that holds all game state, eliminating the need to pass context between functions:

- **world_t**: Global container for game state, entities, and resources
  - `game_t game`: Maze data, score, level, timing
  - `entity_t pacman`: Player character
  - `entity_t ghosts[4]`: All ghost entities (Blinky, Inky, Pinky, Sue)
  - `Texture2D game_texture`: Loaded spritesheet
  - `Image game_image`: Used for pixel-level maze parsing

### Entity System

Both Pac-Man and ghosts use the unified `entity_t` structure (game.h:126-147):

- Position tracking: `pos` (pixel coordinates) and `tile` (grid coordinates)
- Movement: `dir`, `next_dir`, `pixels_moved` for smooth sub-tile movement
- Animation: `frame_count`, `frame_index`, sprite coordinate arrays
- Ghost-specific: Function pointers for `chase()`, `scatter()`, `leave()` behaviors

Each ghost has unique AI implemented via function pointers:
- **Blinky**: Directly chases Pac-Man's current position
- **Pinky**: Targets 4 tiles ahead of Pac-Man (with north overflow bug)
- **Inky**: Uses vector math between Blinky and a pivot point ahead of Pac-Man
- **Sue** (Clyde): Chases when far, scatters when close (<8 tiles)

### Movement System

Movement uses a tile-based system with sub-pixel precision (game.c:40-59):

- Entities move in 8-pixel tiles with fractional progress
- `pixels_moved` tracks sub-tile position
- Perpendicular axis snapping prevents drift during turns
- Special handling for reversals to avoid visual gaps
- Tunnel detection and wraparound at maze edges

### Game Loop Structure

The game loop (main.c:80-91) updates and renders in two phases:

1. **Update Phase** (world.c:update_world):
   - Handle input (movement, pause, debug keys)
   - Update ghost state based on level timer (scatter/chase phases)
   - Update Pac-Man position and dot eating
   - Update all ghost positions and AI

2. **Render Phase** (world.c:draw_world):
   - Two camera views: maze camera and sprite camera (offset by TILE/2)
   - Chroma key shader makes black transparent on sprites
   - Debug mode shows tile alignment and target positions

### Maze System

Mazes are procedurally parsed from the spritesheet (maze.c:27-60):

- `read_block()` converts 8x8 pixel blocks to 64-bit masks
- Compares against `DOT_MASK` and `POWER_MASK` constants to identify tiles
- Detects tunnels by checking empty tiles at maze edges
- Each level selects a different vertical section of the spritesheet

### Ghost State Machine

Ghosts cycle through timed phases (world.c:26-54):
- Scatter (7s) → Chase (20s) → Scatter (7s) → Chase (20s) → Scatter (5s) → Chase (20s) → Scatter (5s) → Chase (indefinite)

States: `GHOST_SCATTER`, `GHOST_CHASE`, `GHOST_FRIGHTENED`, `GHOST_EATEN`, `GHOST_IN_HOUSE`, `GHOST_LEAVING_HOUSE`

### Movement Precision

The codebase addresses several subtle movement bugs:

- **Alignment**: Perpendicular axis snapping (game.c:47-51) prevents drift
- **Reversals**: Special logic (pacman.c:143-170) for smooth 180° turns without gaps
- **Turning**: Anchor point adjustment (pacman.c:171-187) prevents misalignment when changing direction
- **Collision**: Uses rounded tile positions (pacman.c:75-112) for accurate wall detection at junctions

## Controls

- Arrow keys: Movement
- P/Space: Pause
- D: Toggle debug mode (shows tile alignment, ghost targets)
- N: Cycle through maze levels
- C: Force ghosts to chase mode
- S: Force ghosts to scatter mode

## Development Notes

- The code includes TODO comments for incomplete features (ghost house logic, level progression)
- Debug mode is invaluable for diagnosing movement issues
- The original game's "overflow bug" is intentionally preserved where Pinky and Inky behave incorrectly when Pac-Man faces north
- Speed values are calibrated to original game timings (see pacman.c:60-73 and ghost.c:216-231)
- Ghost speeds reduce to 50% in tunnels

## File Organization

- `main.c`: Entry point, window setup, game loop, global world state
- `game.h`/`game.c`: Core types, entity movement primitives
- `pacman.h`/`pacman.c`: Pac-Man initialization, movement logic, collision, dot eating
- `ghost.h`/`ghost.c`: Ghost AI, initialization, chase/scatter behaviors
- `maze.h`/`maze.c`: Maze parsing from spritesheet
- `draw.h`/`draw.c`: All rendering (maze, sprites, debug overlays)
- `world.h`/`world.c`: High-level update/render coordination, input handling, ghost phase timing
