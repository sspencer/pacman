//
// Created by Steve Spencer on 12/11/25.
//

#include "ghost.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    EMPTY_MASK = 0,
    NORTH_MASK = 1,
    SOUTH_MASK = 2,
    EAST_MASK = 4,
    WEST_MASK = 8,
} dir_mask_t;

static int maze_top(int level) {
    // TODO change from level to board
    if (level == 2) { // should be "board" not level
        return 4;
    }

    return 1;
}

Vector2 blinky_chase(void) {
    return (Vector2){(float)world.pacman.tile.x, (float)world.pacman.tile.y};
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
        .start_state = GHOST_SCATTER,
        .chase = blinky_chase,
        .leave = blinky_leave,
        .scatter = blinky_scatter
    };
    return data;
}

Vector2 inky_chase(void) {
    return (Vector2){(float)world.pacman.tile.x, (float)world.pacman.tile.y};
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
        .start_state = GHOST_SCATTER, // TODO update to GHOST_IN_HOUSE
        .chase = inky_chase,
        .leave = inky_leave,
        .scatter = inky_scatter
    };

    data.start = (Vector2){18, 11}; // TODO remove

    return data;
}

Vector2 pinky_chase(void) {
    return (Vector2){world.pacman.tile.x, world.pacman.tile.y};
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
        .start_state = GHOST_SCATTER, // TODO update to GHOST_IN_HOUSE
        .chase = pinky_chase,
        .leave = pinky_leave,
        .scatter = pinky_scatter
    };
    data.start = (Vector2){9, 11}; // TODO remove

    return data;
}

Vector2 sue_chase(void) {
    return (Vector2){world.pacman.tile.x, world.pacman.tile.y};
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
        .start_state = GHOST_SCATTER, // TODO update to GHOST_IN_HOUSE
        .chase = sue_chase,
        .leave = sue_leave,
        .scatter = sue_scatter
    };
    data.start = (Vector2){9, 4}; // TODO remove

    return data;
}

void init_ghost(entity_t *entity, ghost_data_t data) {
    entity->tile.x = data.start.x;
    entity->tile.y = data.start.y;
    entity->pos.x = entity->tile.x * TILE;
    entity->pos.y = entity->tile.y * TILE;
    entity->pixels_moved = 0;

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

    entity->frame_count = 0;
    entity->frame_index = 0;

    entity->state = data.start_state;
    entity->pixels_moved_in_dir = 0;
    entity->chase = data.chase;
    entity->leave = data.leave;
    entity->scatter = data.scatter;
}

static void update_ghost_frame(entity_t *g) {
    const int frames_per_state = 15;
    const int cycle_length = 2 * frames_per_state; // 30 frames for full cycle
    const int frame_in_cycle = g->frame_count % cycle_length;

    if (frame_in_cycle < frames_per_state) {
        g->frame_index = 0; // First pose
    } else {
        g->frame_index = 1;
    }
    
    g->frame_count++;
    if (g->frame_count > cycle_length) g->frame_count = 0;
}

//    int level = world.game.level;
static float ghost_speed(int level) {
    // 96% of pacman's speed
    static const float speed[] = {
        84.48f,
        92.928f,
        92.928f,
        92.928f,
        101.376f   // used for level 5+
    };
    const size_t idx = (level < 1) ? 0 : (level > 4) ? 4 : (size_t)(level - 1);
    return speed[idx] / 60.0f; // convert to pixels per frame
}

static dir_t opposite_dir(dir_t dir) {
    switch (dir) {
        case DIR_NORTH: return DIR_SOUTH;
        case DIR_SOUTH: return DIR_NORTH;
        case DIR_EAST: return DIR_WEST;
        case DIR_WEST: return DIR_EAST;
        default: return DIR_WEST;
    }
}

static float dist_between(Vector2 a, Vector2 b) {
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

static dir_t choose_ghost_dir(entity_t *g, Vector2 target) {
    int valid_dirs = 0;

    for (int i = 0; i < DIR_COUNT; i++) {
        if (i == opposite_dir(g->dir)) continue;
        Vector2 next_tile = get_next_tile(g, i);
        if (next_tile.x < 0 || next_tile.x >= GAME_WIDTH || next_tile.y < 0 || next_tile.y >= GAME_HEIGHT) continue;
        if (world.game.maze[(int)next_tile.y][(int)next_tile.x] == TILE_WALL) continue;
        valid_dirs |= 1 << i;
    }

    if (valid_dirs == 0) return g->dir;

    if (g->pixels_moved_in_dir < TILE) return g->dir;

    dir_t best_dir = 0;
    float min_distance = MAXFLOAT;

    for (int i = 0; i < DIR_COUNT; i++) {
        if (valid_dirs & 1 << i) {
            const dir_t dir = (dir_t)i;
            Vector2 next_tile = get_next_tile(g, i);
            float dist = dist_between(target, next_tile);
            if (dist < min_distance) {
                min_distance = dist;
                best_dir = dir;
            }
        }
    }

    return best_dir;
}

static void move_ghost(entity_t *g, Vector2 vel, float speed) {
    if (vel.x != 0 || vel.y != 0) {
        g->pixels_moved += speed;

        const float clamp = fminf(g->pixels_moved, TILE);

        g->pos.x = (g->tile.x * TILE + (vel.x * clamp));
        g->pos.y = (g->tile.y * TILE + (vel.y * clamp));
    }
}
static void update_ghost(entity_t *g) {
    update_ghost_frame(g);
    if (g->pixels_moved >= TILE) {
        set_next_tile(g, g->dir);
        g->pixels_moved = 0;
    }

    float speed = ghost_speed(world.game.level);

    // if (g.state == GHOST_SCATTER)
    const Vector2 target = g->scatter();

    dir_t current_dir = g->dir;
    g->dir = choose_ghost_dir(g, target);

    if (current_dir == g->dir) {
         g->pixels_moved_in_dir += speed;
    } else {
         g->pixels_moved_in_dir = speed;
    }

    move_entity(g, speed);
}

void update_ghosts() {
    for (int i = 0; i < GHOST_COUNT; i+=1) {
        update_ghost(&world.ghosts[i]);
    }
}