//
// Created by Steve Spencer on 2/20/26.
//

#ifndef PACMAN_PRNG_H
#define PACMAN_PRNG_H

#include "game.h"

#define UP_MASK 0b0001
#define RIGHT_MASK 0b0010
#define DOWN_MASK 0b0100
#define LEFT_MASK 0b1000


void init_prng();
void reset_prng();
Dir choose_frightened_direction(Dir current_dir, unsigned int allowed_dirs);
Dir choose_random_direction(Dir current_dir, unsigned int allowed_dirs);

#endif //PACMAN_PRNG_H