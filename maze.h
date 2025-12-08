//
// Created by Steve Spencer on 12/4/25.
//

#ifndef PACMAN_MAZE_H
#define PACMAN_MAZE_H
#include <raylib.h>

// Forward declaration allows usage of "struct Game *" without including the full header
struct Game;

void debugMaze(struct Game *game);
void map_maze(struct Game *game, Image *image);

#endif //PACMAN_MAZE_H