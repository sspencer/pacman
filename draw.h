//
// Created by Steve Spencer on 12/4/25.
//

#ifndef PACMAN_DRAW_H
#define PACMAN_DRAW_H

#include <raylib.h>

#include "pacman.h"

void draw_maze(void);
void draw_ghosts(void);
void draw_pacman(void);
void draw_checkerboard(void);
void draw_text(const char *text, int x, int y, Color color);

#endif //PACMAN_DRAW_H