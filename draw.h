//
// Created by Steve Spencer on 1/8/26.
//

#ifndef PACMAN_DRAW_H
#define PACMAN_DRAW_H

#include "game.h"

void draw_maze(void);

void draw_pacman(Actor *);

void draw_ms_pacman(Actor *);

void draw_ghosts(void);

void draw_ghost_score(int, int, int);

void draw_checkerboard(void);

void draw_scores(void);

void draw_score_labels(void);

void draw_level(void);
#endif // PACMAN_DRAW_H
