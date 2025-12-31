//
// Created by Steve Spencer on 12/11/25.
//

#ifndef PACMAN_GHOST_H
#define PACMAN_GHOST_H
#include "game.h"

ghost_data_t blinky_data();
ghost_data_t inky_data();
ghost_data_t pinky_data();
ghost_data_t sue_data();

void init_ghost(entity_t *entity, ghost_data_t data);
void update_ghosts(void);

#endif //PACMAN_GHOST_H