//
// Created by Steve Spencer on 12/4/25.
//
#include <raylib.h>
#include <stdio.h>

#include "game.h"

#define DOT_MASK 103481868288
#define POWER_MASK 4359202964317896252
#define DOOR_MASK 16776960


static unsigned long int read_block(const Image *img, const int startX, const int startY) {
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

void map_maze(game_t *game) {
    const int offset = game->level * GAME_HEIGHT * TILE;

    // find walls/dots/power ups
    for (int y = 0; y < GAME_HEIGHT; y++) {
        for (int x = 0; x < GAME_WIDTH; x++) {
            const unsigned long int pixel = read_block(&world.image, x * TILE, y * TILE + offset);

            switch (pixel) {
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
                default:
                    printf("???");
                    break;
            }
        }
        printf("\n");
    }
}
