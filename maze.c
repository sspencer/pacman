//
// Created by Steve Spencer on 12/4/25.
//
#include <raylib.h>
#include <stdio.h>

#include "game.h"

#define DOT_MASK 103481868288
#define POWER_MASK 4359202964317896252
#define DOOR_MASK 16776960

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
    unsigned long int result = 0L;

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
 * @param[in,out] game A pointer to the game structure containing the current
 *                     level and maze data that will be updated.
 */
void map_maze(game_t *game) {
    const int offset = game->level * GAME_HEIGHT * TILE;

    // find walls/dots/power ups
    for (int y = 0; y < GAME_HEIGHT; y++) {
        for (int x = 0; x < GAME_WIDTH; x++) {
            const unsigned long int mask = compute_mask(&world.image, x * TILE, y * TILE + offset);

            switch (mask) {
                case 0: game->maze[y][x] = TILE_EMPTY;
                    break;
                case DOT_MASK: game->maze[y][x] = TILE_DOT;
                    break;
                case POWER_MASK: game->maze[y][x] = TILE_POWER;
                    break;
                case DOOR_MASK: game->maze[y][x] = TILE_DOOR;
                    break;
                default: game->maze[y][x] = TILE_WALL;
            }
        }
    }

    // find tunnels
    for (int y = 0; y < GAME_HEIGHT; y++) {
        if ((game->maze[y][0] == TILE_EMPTY) &&
            (game->maze[y][1] == TILE_EMPTY || game->maze[y][1] == TILE_DOT) &&
            (game->maze[y][2] == TILE_EMPTY || game->maze[y][2] == TILE_DOT)) {
            game->maze[y][0] = TILE_TUNNEL;
        }

        if ((game->maze[y][GAME_WIDTH - 3] == TILE_EMPTY || game->maze[y][GAME_WIDTH - 3] == TILE_DOT) &&
            (game->maze[y][GAME_WIDTH - 2] == TILE_EMPTY || game->maze[y][GAME_WIDTH - 2] == TILE_DOT) &&
            game->maze[y][GAME_WIDTH - 1] == TILE_EMPTY) {
            game->maze[y][GAME_WIDTH - 1] = TILE_TUNNEL;
        }
    }
}

void debug_maze(const game_t *game) {
    for (int y = 0; y < GAME_HEIGHT; y++) {
        for (int x = 0; x < GAME_WIDTH; x++) {
            switch (game->maze[y][x]) {
                case TILE_WALL:
                    printf("###");
                    break;
                case TILE_DOT:
                    printf(" + ");
                    break;
                case TILE_POWER:
                    printf(" @ ");
                    break;
                case TILE_EMPTY:
                    printf("   ");
                    break;
                case TILE_TUNNEL:
                    printf("<|>");
                    break;
                case TILE_DOOR:
                    printf("---");
                    break;
                default:
                    printf("???");
                    break;
            }
        }
        printf("\n");
    }
}
