//
// Created by Steve Spencer on 12/4/25.
//

#ifndef PACMAN_GAME_H
#define PACMAN_GAME_H

#include <raylib.h>

#define DEBUG 0
#define SCREEN_HEIGHT 36
#define SCREEN_WIDTH  28
#define GAME_WIDTH 28
#define GAME_HEIGHT 31
#define TOP_PADDING 3
#define ZOOM 3
#define SIZE 8
#define PIXEL (SIZE * ZOOM)

#define DOT_MASK 103481868288
#define POWER_MASK 4359202964317896252

typedef enum {
    TILE_WALL,
    TILE_DOT,
    TILE_POWER,
    TILE_EMPTY,
    TILE_TUNNEL
} tile_t;

typedef struct {
    int level;
    int maze[SCREEN_HEIGHT][SCREEN_WIDTH];
    bool paused;
} game_t;


#endif //PACMAN_GAME_H