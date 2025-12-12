//
// Created by Steve Spencer on 12/11/25.
//

#include "ghost.h"

#include <math.h>
#include <stdlib.h>

/*
Blinky:
 	sprite:       Vec2i{X: 520, Y: 64},
    startingTile: Vec2i{X: 13, Y: 11},
    startDir: West
Inky
    sprite:       Vec2i{X: 520, Y: 96},
    startingTile: Vec2i{X: 12, Y: 14},
    startingDir:  Up,
Pinky:
    sprite:       Vec2i{X: 520, Y: 80},
    startingTile: Vec2i{X: 14, Y: 14},
    startingDir:  Down,
Sue:
    sprite:       Vec2i{X: 520, Y: 112},
    startingTile: Vec2i{X: 16, Y: 14},
    startingDir:  Up,
*/

static int maze_top(int level) {
    // TODO change from level to board
    if (level == 2) { // should be "board" not level
        return 4;
    }

    return 1;
}

Vector2 blinky_chase(void) {
    return (Vector2){(float)world.pacman.tx, (float)world.pacman.ty};
}

bool blinky_leave(void) {
    return true;
}

Vector2 blinky_scatter(void) {
    return (Vector2){26, (float)maze_top(world.game.level)};
}

ghost_data_t blinky_data() {
    static ghost_data_t data = {
        .sprite = {0, 64},
        .start = {13, 11},
        .start_dir = DIR_WEST,
        .chase = blinky_chase,
        .leave = blinky_leave,
        .scatter = blinky_scatter
    };
    return data;
}

Vector2 inky_chase(void) {
    return (Vector2){(float)world.pacman.tx, (float)world.pacman.ty};
}

bool inky_leave(void) {
    return true;
}

Vector2 inky_scatter(void) {
    return (Vector2){1, 29};
}

ghost_data_t inky_data() {
    ghost_data_t data = {
        .sprite = {0, 96},
        .start = {12, 14},
        .start_dir = DIR_NORTH,
        .chase = inky_chase,
        .leave = inky_leave,
        .scatter = inky_scatter
    };
    return data;
}

Vector2 pinky_chase(void) {
    return (Vector2){(float)world.pacman.tx, (float)world.pacman.ty};
}

bool pinky_leave(void) {
    return true;
}

Vector2 pinky_scatter(void) {
    return (Vector2){1, (float)maze_top(world.game.level)};
}

ghost_data_t pinky_data() {
    ghost_data_t data = {
        .sprite = {0, 80},
        .start = {14, 14},
        .start_dir = DIR_SOUTH,
        .chase = pinky_chase,
        .leave = pinky_leave,
        .scatter = pinky_scatter
    };
    return data;
}

Vector2 sue_chase(void) {
    return (Vector2){(float)world.pacman.tx, (float)world.pacman.ty};
}

bool sue_leave(void) {
    return true;
}

Vector2 sue_scatter(void) {
    return (Vector2){26, 29};
}
ghost_data_t sue_data() {
    ghost_data_t data = {
        .sprite = {0, 112},
        .start = {16, 14},
        .start_dir = DIR_NORTH,
        .chase = sue_chase,
        .leave = sue_leave,
        .scatter = sue_scatter
    };
    return data;
}

void init_ghost(entity_t *entity, ghost_data_t data) {
    entity->tx = (int)data.start.x;
    entity->ty = (int)data.start.y;
    entity->x = (float)entity->tx * TILE;
    entity->y = (float)entity->ty * TILE;

    entity->sprite_x[DIR_EAST] = 456;
    entity->sprite_y[DIR_EAST] = (int)data.sprite.y;
    entity->sprite_x[DIR_WEST] = 488;
    entity->sprite_y[DIR_WEST] = (int)data.sprite.y;
    entity->sprite_x[DIR_NORTH] = 520;
    entity->sprite_y[DIR_NORTH] = (int)data.sprite.y;
    entity->sprite_x[DIR_SOUTH] = 552;
    entity->sprite_y[DIR_SOUTH] = (int)data.sprite.y;

    entity->dir = data.start_dir;
    entity->next_dir = data.start_dir;

    entity->frame_count = 2;
    entity->frame_index = 0;

    entity->chase = data.chase;
    entity->leave = data.leave;
    entity->scatter = data.scatter;
}

void update_ghosts() {
    // for (int i = 0; i < GHOST_COUNT; i+=1) {
    //     entity_t *g = &world.ghosts[i];
    // }
}