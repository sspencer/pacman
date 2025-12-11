//
// Created by Steve Spencer on 12/4/25.
//

#ifndef PACMAN_DRAW_H
#define PACMAN_DRAW_H

#include <raylib.h>

#include "entity.h"

void draw_maze(const game_t *game);
void draw_player(const entity_t *player);

#endif //PACMAN_DRAW_H