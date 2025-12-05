//
// Created by Steve Spencer on 12/4/25.
//
#include <raylib.h>
#include <stdio.h>

#include "game.h"

long int readPixelArea(Image *img, int startX, int startY, int width, int height) {
    long int result = 0L;

    for (int y = startY; y < startY + height; y++) {
        for (int x = startX; x < startX + width; x++) {
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

void createMap(struct Game *game, Image *image) {
    int offset = game->level * GAME_HEIGHT * SIZE;

    // find walls/dots/power ups
    for (int y = 0; y < GAME_HEIGHT; y++) {
        for (int x = 0; x < GAME_WIDTH; x++) {
            long int pixel = readPixelArea(image, x * SIZE, y * SIZE + offset, SIZE, SIZE);
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

void drawMaze(Texture2D texture, int level) {
    float x = (GAME_WIDTH * SIZE) + SIZE / 2.0;
    float y = ((float) level * GAME_HEIGHT * SIZE);
    float w = GAME_WIDTH * SIZE;
    float h = GAME_HEIGHT * SIZE;

    Rectangle src = (Rectangle){x, y, w, h};
    Rectangle dst = (Rectangle){0, 0, w * ZOOM, h * ZOOM};

    DrawTexturePro(texture, src, dst, (Vector2){0, 0}, 0, WHITE);
}

void printMaze(struct Game *game) {
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
