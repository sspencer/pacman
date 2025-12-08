//
// Created by Steve Spencer on 12/4/25.
//

#ifndef PACMAN_GAME_H
#define PACMAN_GAME_H

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

enum Tile {
    WALL, DOT, POWER, EMPTY, TUNNEL
};

struct Game {
    int level;
    int maze[SCREEN_HEIGHT][SCREEN_WIDTH];
};

#endif //PACMAN_GAME_H