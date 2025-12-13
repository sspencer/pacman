//
// Created by Steve Spencer on 12/12/25.
//
#include "game.h"

#include <math.h>

void set_next_tile(entity_t *e, dir_t dir) {
    switch (dir) {
        case DIR_NORTH: e->tile.y--; break;
        case DIR_SOUTH: e->tile.y++; break;
        case DIR_EAST: e->tile.x++; break;
        default:
            e->tile.x--;
    }
}

Vector2 get_next_tile(entity_t *e, dir_t dir) {
    Vector2 tile = (Vector2){e->tile.x, e->tile.y};
    switch (dir) {
        case DIR_NORTH: tile.y--; break;
        case DIR_SOUTH: tile.y++; break;
        case DIR_EAST: tile.x++; break;
        default: tile.x--;
    }
    return tile;
}

void move_entity(entity_t *e, float speed) {
    e->pixels_moved += speed;

    const float clamp = fminf(e->pixels_moved, TILE);

    switch(e->dir) {
        case DIR_NORTH: e->pos.y = e->tile.y * TILE - clamp; break;
        case DIR_SOUTH: e->pos.y = e->tile.y * TILE + clamp; break;
        case DIR_EAST: e->pos.x = e->tile.x * TILE + clamp; break;
        default: e->pos.x = e->tile.x * TILE - clamp; break;
    }
}
