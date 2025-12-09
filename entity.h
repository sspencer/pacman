//
// Created by Steve Spencer on 12/8/25.
//

#ifndef PACMAN_ENTITY_H
#define PACMAN_ENTITY_H

#include <raylib.h>
#include "game.h"

void init_player(entity_t *entity);
void update_player(game_t *game, entity_t *p);

#endif //PACMAN_ENTITY_H