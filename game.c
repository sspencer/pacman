//
// Created by Steve Spencer on 12/12/25.
//
#include "game.h"

#include <math.h>

bool is_in_tunnel(Vector2 pos) {
    const int ty = (int)pos.y;
    if (ty < 0 || ty >= GAME_HEIGHT) return false;

    const bool row_has_tunnel = world.game.maze[ty][0] == TILE_TUNNEL;

    if (!row_has_tunnel) return false;

    return ((int)pos.x <= 0 || (int)pos.x >= GAME_WIDTH-1);
}

void set_next_tile(entity_t *e, dir_t dir) {
    switch (dir) {
        case DIR_NORTH: e->tile.y--; break;
        case DIR_SOUTH: e->tile.y++; break;
        case DIR_EAST: e->tile.x++; break;
        default:
            e->tile.x--;
    }
}

Vector2 get_next_tile(const entity_t *e, dir_t dir) {
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
    // Keep entity centered in corridors by snapping the perpendicular axis
    // to the exact tile grid each frame. This avoids gradual drift and ensures
    // clean alignment when turning (e.g., SOUTH -> WEST).
    if (e->dir == DIR_EAST || e->dir == DIR_WEST) {
        e->pos.y = e->tile.y * TILE;
    } else { // moving NORTH or SOUTH
        e->pos.x = e->tile.x * TILE;
    }

    switch(e->dir) {
        case DIR_NORTH: e->pos.y = e->tile.y * TILE - clamp; break;
        case DIR_SOUTH: e->pos.y = e->tile.y * TILE + clamp; break;
        case DIR_EAST:  e->pos.x = e->tile.x * TILE + clamp; break;
        default:        e->pos.x = e->tile.x * TILE - clamp; break;
    }
}

bool is_in_doorway(int x, int y) {
    return world.game.maze[y][x] == TILE_DOOR;
}

bool is_in_house(int x, int y) {
    return ((x >= 11 && x <= 16) && (y >= 13 && y <= 15)) || is_in_doorway(x, y);
}

