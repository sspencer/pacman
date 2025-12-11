//
// Created by Steve Spencer on 12/4/25.
//
#include <raylib.h>
#include <stdio.h>

#include "game.h"

static long int read_block(const Image *img, int startX, int startY) {
    long int result = 0L;

    for (int y = startY; y < startY + SIZE; y++) {
        for (int x = startX; x < startX + SIZE; x++) {
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
    int offset = game->level * GAME_HEIGHT * SIZE;

    // find walls/dots/power ups
    for (int y = 0; y < GAME_HEIGHT; y++) {
        for (int x = 0; x < GAME_WIDTH; x++) {
            long int pixel = read_block(&game_image, x * SIZE, y * SIZE + offset);
            switch (pixel) {
                case 0: game->maze[y][x] = TILE_EMPTY;
                    break;
                case DOT_MASK: game->maze[y][x] = TILE_DOT;
                    break;
                case POWER_MASK: game->maze[y][x] = TILE_POWER;
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
