//
// Created by Steve Spencer on 1/8/26.
//

#include <raylib.h>
#include "maze.h"

#include <stdio.h>
#include "game.h"

#define EMPTY_MASK 0UL
#define DOT_MASK 103481868288UL
#define POWER_MASK 4359202964317896252UL
#define DOOR_MASK 16776960UL

/**
 * @brief Computes a binary mask for a tile by analyzing its pixels within the image.
 *
 * This function scans a square tile of dimensions `TILE x TILE` starting
 * at the specified coordinates in the provided image. For each pixel,
 * it determines if it is non-black (based on its RGB values), and sets
 * the corresponding bit in a binary mask. The resulting mask can be
 * used to classify the tile type or for other purposes in the game logic.
 *
 * @param[in] img A pointer to the image containing the maze or game world.
 * @param[in] startX The X-coordinate of the top-left corner of the tile in pixels.
 * @param[in] startY The Y-coordinate of the top-left corner of the tile in pixels.
 * @return A binary mask that represents the non-black pixels within the specified tile area.
 */
static unsigned long int compute_mask(const Image *img, const int startX, const int startY) {
    uint64_t result = 0UL;

    for (int y = startY; y < startY + TILE; y++) {
        for (int x = startX; x < startX + TILE; x++) {
            // Get the color of the pixel at position (pixelX, pixelY)
            result <<= 1;

            const Color color = GetImageColor(*img, x, y);
            if (color.r > 0 || color.g > 0 || color.b > 0) {
                result |= 1;
            }
        }
    }

    return result;
}

int get_maze_top(const int level) {
    if (get_maze_num(level) == 1) {
        return 4 * TILE;
    }

    return 1 * TILE;
}

int get_maze_num(const int level) {
    if (game.is_pacman) {
        return 4;
    }
    if (level <= 2) return 0;            // Levels 1-2: Maze 1 (index 0)
    if (level <= 5) return 1;            // Levels 3-5: Maze 2 (index 1)
    if (level <= 9) return 2;            // Levels 6-9: Maze 3 (index 2)
    if (level <= 13) return 3;           // Levels 10-13: Maze 4 (index 3)

    // After level 13, alternate between mazes 3 and 4 every four levels.
    // Level 14-17: Maze 3 (index 2)
    // Level 18-21: Maze 4 (index 3)
    // Level 22-25: Maze 3 (index 2)
    // ...
    int cycle = (level - 14) / 4;
    return (cycle % 2 == 0) ? 2 : 3;
}

static void set_tunnels(int x1, int x2, int y) {
    uint8_t (*maze)[GAME_WIDTH] = game.maze;
    for (int x = 0; x <= x1; x++) {
        maze[y][x] = TILE_TUNNEL;
    }

    for (int x = x2; x < GAME_WIDTH; x++) {
        maze[y][x] = TILE_TUNNEL;
    }
}

/**
 * @brief Populates the maze layout for the current game level by analyzing
 *        the world image to identify various tile types such as walls, dots,
 *        power-ups, doors, and tunnels.
 *
 * This function processes the game level defined in the `game` parameter and
 * determines the tile type for each position in the maze using predefined
 * masks and tile dimensions. Additionally, it detects horizontal tunnels at
 * the edges of the maze and updates the maze accordingly.
 *
 * @param[in] level The level to render.  Maze and level are not one-to-one.
 */
void init_maze(const int level) {
    const int maze_num = get_maze_num(level);
    const Image image = LoadImageFromTexture(game.sprite_texture);
    const int offset = maze_num * GAME_HEIGHT * TILE;

    game.dots_eaten = 0;
    game.dots_remaining = 0;

    uint8_t (*maze)[GAME_WIDTH] = game.maze;

    // find walls/dots/power ups
    for (int y = 0; y < GAME_HEIGHT; y++) {
        for (int x = 0; x < GAME_WIDTH; x++) {
            const unsigned long int mask = compute_mask(&image, x * TILE, y * TILE + offset);

            switch (mask) {
                case EMPTY_MASK: maze[y][x] = TILE_EMPTY; break;
                case DOT_MASK: maze[y][x] = TILE_DOT; game.dots_remaining++; break;
                case POWER_MASK: maze[y][x] = TILE_POWER; game.dots_remaining++; break;
                case DOOR_MASK: maze[y][x] = TILE_DOOR; break;
                default: maze[y][x] = TILE_WALL; break;
            }
        }
    }

    if (maze_num == 0) {
        set_tunnels(1, 26, 8);
        set_tunnels(1, 26, 17);
    } else if (maze_num == 1) {
        set_tunnels(4, 23, 1);
        set_tunnels(1, 26, 23);
    } else if (maze_num == 2) {
        set_tunnels(0, 27, 9);
    } else if (maze_num == 3) {
        set_tunnels(1, 26, 13);
        set_tunnels(1, 26, 16);
    } else {
        // pacman
        set_tunnels(4, 23, 14);
    }

    UnloadImage(image);
}

bool is_ghost_wall(int x, int y) {
    int maze_num = get_maze_num(game.level);
    if (maze_num < 4) {
        return false;
    }

    return (y == 10 || y == 22) && (x == 12 || x == 15); // pacman maze only

    /*
    switch (maze_num) {
        case 0: return (y == 10 || y == 22) && (x == 9 || x == 18);
        case 1: return (y == 10 || y == 22) && (x == 11 || x == 16);
        case 2: return (y == 10 || y == 22) && (x == 9 || x == 18);
        default: return (y == 10 || y == 22) && (x == 12 || x == 15); // maze 3 or pacman
    }
    */
}
