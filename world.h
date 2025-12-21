//
// Created by Steve Spencer on 12/12/25.
//

#ifndef PACMAN_UPDATE_H
#define PACMAN_UPDATE_H

#include <raylib.h>
#include "game.h"

void draw_world(float, Shader);
void update_world(void);
void set_ghost_state(state_t);

#endif //PACMAN_UPDATE_H