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

void map_maze(struct Game *game, const Image *image) {
    int offset = game->level * GAME_HEIGHT * SIZE;

    // find walls/dots/power ups
    for (int y = 0; y < GAME_HEIGHT; y++) {
        for (int x = 0; x < GAME_WIDTH; x++) {
            long int pixel = read_block(image, x * SIZE, y * SIZE + offset);
            switch (pixel) {
                case 0: game->maze[y][x] = EMPTY;
                    break;
                case DOT_MASK: game->maze[y][x] = DOT;
                    break;
                case POWER_MASK: game->maze[y][x] = POWER;
                    break;
                default: game->maze[y][x] = WALL;
            }
        }
    }

    // find tunnels
    for (int y = 0; y < GAME_HEIGHT; y++) {
        if ((game->maze[y][0] == EMPTY) &&
            (game->maze[y][1] == EMPTY || game->maze[y][1] == DOT) &&
            (game->maze[y][2] == EMPTY || game->maze[y][2] == DOT)) {
            game->maze[y][0] = TUNNEL;
        }

        if ((game->maze[y][GAME_WIDTH - 3] == EMPTY || game->maze[y][GAME_WIDTH - 3] == DOT) &&
            (game->maze[y][GAME_WIDTH - 2] == EMPTY || game->maze[y][GAME_WIDTH - 2] == DOT) &&
            game->maze[y][GAME_WIDTH - 1] == EMPTY) {
            game->maze[y][GAME_WIDTH - 1] = TUNNEL;
        }
    }
}

void debugMaze(const struct Game *game) {
    for (int y = 0; y < GAME_HEIGHT; y++) {
        for (int x = 0; x < GAME_WIDTH; x++) {
            switch (game->maze[y][x]) {
                case WALL:
                    printf("###");
                    break;
                case DOT:
                    printf(" + ");
                    break;
                case POWER:
                    printf(" @ ");
                    break;
                case EMPTY:
                    printf("   ");
                    break;
                case TUNNEL:
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
