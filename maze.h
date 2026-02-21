//
// Created by Steve Spencer on 1/8/26.
//

#ifndef PACMAN_MAZE_H
#define PACMAN_MAZE_H

// TODO remove level get it from game
void init_maze(int level);
int get_maze_num(int level);
bool is_ghost_wall(int, int);

#endif // PACMAN_MAZE_H

