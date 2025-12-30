//
// Created by Steve Spencer on 12/11/25.
//

#include "ghost.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static int maze_top(int level) {
    // TODO change from level to board
    if (level == 2) { // should be "board" not level
        return 4;
    }

    return 1;
}

Vector2 blinky_chase(void) {
    return (Vector2){world.pacman.tile.x, world.pacman.tile.y};
}

bool blinky_leave(void) {
    return true;
}

Vector2 blinky_scatter(void) {
    return (Vector2){26, (float)maze_top(world.game.level)};
}

ghost_data_t blinky_data() {
    static ghost_data_t data = {
        .sprite_y = 64,
        .start = {13, 11},
        .start_dir = DIR_WEST,
        .start_state = STATE_SCATTER,
        .chase = blinky_chase,
        .leave = blinky_leave,
        .scatter = blinky_scatter
    };
    return data;
}

Vector2 inky_chase(void) {
    entity_t *p = &world.pacman;
    entity_t *blinky = &world.ghosts[GHOST_BLINKY];

    float vx = 0.0f, vy = 0.0f;

    // reproduces original chase bug for NORTH
    // to remove "bug", set vx=0
    switch (p->dir) {
        case DIR_NORTH: vx = -2.0f, vy = 2.0f; break;
        case DIR_SOUTH: vy = 2.0f; break;
        case DIR_EAST: vx = 2.0f; break;
        default: vx = -2.0f; break;
    }

    const Vector2 pivot = (Vector2){p->tile.x + vx, p->tile.y + vy};
    const float dx = pivot.x - p->tile.x;
    const float dy = pivot.y - p->tile.y;

    return (Vector2){
        blinky->tile.x + 2 * dx,
        blinky->tile.y + 2 * dy
    };
}

bool inky_leave(void) {
    if (world.game.dots_eaten > 30) {
        return world.game.level_time > 7;
    }

    return false;
}

Vector2 inky_scatter(void) {
    return (Vector2){1, 29};
}

ghost_data_t inky_data() {
    ghost_data_t data = {
        .sprite_y = 96,
        .start = {12, 14},
        .start_dir = DIR_NORTH,
        .start_state = STATE_IN_HOUSE,
        .chase = inky_chase,
        .leave = inky_leave,
        .scatter = inky_scatter
    };

    return data;
}

Vector2 pinky_chase(void) {
    entity_t *p = &world.pacman;

    float vx = 0.0f, vy = 0.0f;

    // reproduces original chase bug for NORTH
    // to remove "bug", set vx=0
    switch (p->dir) {
        case DIR_NORTH: vx = -4.0f, vy = -4.0f; break;
        case DIR_SOUTH: vy = 4.0f; break;
        case DIR_EAST: vx = 4.0f; break;
        default: vx = -4.0f; break;
    }

    const Vector2 pivot = (Vector2){p->tile.x + vx, p->tile.y + vy};
    return pivot;
}

bool pinky_leave(void) {
    return world.game.level_time > 1;
}

Vector2 pinky_scatter(void) {
    return (Vector2){1, (float)maze_top(world.game.level)};
}

ghost_data_t pinky_data() {
    ghost_data_t data = {
        .sprite_y = 80,
        .start = {14, 14},
        .start_dir = DIR_SOUTH,
        .start_state = STATE_IN_HOUSE,
        .chase = pinky_chase,
        .leave = pinky_leave,
        .scatter = pinky_scatter
    };

    return data;
}

Vector2 sue_scatter(void) {
    return (Vector2){26, 29};
}

Vector2 sue_chase(void) {
    entity_t *p = &world.pacman;
    entity_t *g = &world.ghosts[GHOST_SUE];
    float dx = p->tile.x - g->tile.x;
    float dy = p->tile.y - g->tile.y;
    float dist = sqrtf(dx * dx + dy * dy);
    if (dist > 8) {
        return p->tile;
    }

    return sue_scatter();
}

bool sue_leave(void) {
    if (world.game.dots_eaten > 60) {
        return world.game.level_time > 15;
    }

    return false;
}

ghost_data_t sue_data() {
    ghost_data_t data = {
        .sprite_y = 112,
        .start = {16, 14},
        .start_dir = DIR_NORTH,
        .start_state = STATE_IN_HOUSE,
        .chase = sue_chase,
        .leave = sue_leave,
        .scatter = sue_scatter
    };

    return data;
}

void init_ghost(entity_t *entity, ghost_data_t data) {
    entity->tile.x = data.start.x;
    entity->tile.y = data.start.y;
    entity->pos.x = entity->tile.x * TILE;
    entity->pos.y = entity->tile.y * TILE;
    entity->pixels_moved = 0;

    float bounce = 0;
    if (data.start_dir == DIR_SOUTH) {
        bounce = M_PI / 2;
    }
    entity->bounce = bounce;

    entity->sprite[DIR_EAST] = (Vector2){456, data.sprite_y};
    entity->sprite[DIR_WEST] = (Vector2){488, data.sprite_y};
    entity->sprite[DIR_NORTH] = (Vector2){520,data.sprite_y};
    entity->sprite[DIR_SOUTH] = (Vector2){552, data.sprite_y};

    entity->dir = data.start_dir;
    entity->next_dir = data.start_dir;

    entity->sprite_type = SPRITE_ENTITY;
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
static float ghost_speed(entity_t *g) {
    const int level = world.game.level;
    // 96% of pacman's speed
    static const float speed_table[] = {
        84.48f,
        92.928f,
        92.928f,
        92.928f,
        101.376f   // used for level 5+
    };
    const size_t idx = (level < 1) ? 0 : (level > 4) ? 4 : (size_t)(level - 1);

    float speed = speed_table[idx];
    if (is_in_tunnel(g->tile)) speed *= 0.5f;
    return speed / 60.0f; // convert to pixels per frame
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

static bool is_valid_move(Vector2 tile, bool leaving_house) {
    const int x = (int)tile.x, y = (int)tile.y;

    if (x < 0 || x >= GAME_WIDTH || y < 0 || y >= GAME_HEIGHT) return false;

    tile_t t = world.game.maze[y][x];
    if (t == TILE_WALL) return false;
    if (t == TILE_DOOR) return leaving_house;

    return true;
}

static dir_t choose_ghost_dir(entity_t *g, Vector2 target) {
    int valid_dirs = 0;

    for (int i = 0; i < DIR_COUNT; i++) {
        if (i == opposite_dir(g->dir)) continue;
        Vector2 next_tile = get_next_tile(g, i);
        if (is_valid_move(next_tile, g->state == STATE_LEAVING_HOUSE)) {
            valid_dirs |= 1 << i;
        }
    }

    if (valid_dirs == 0) return g->dir;

    if (g->pixels_moved_in_dir < TILE) return g->dir;

    dir_t best_dir = 0;
    float min_distance = MAXFLOAT;

    for (int i = 0; i < DIR_COUNT; i++) {
        if (valid_dirs & 1 << i) {
            const dir_t dir = (dir_t)i;
            Vector2 next_tile = get_next_tile(g, i);
            const float dist = dist_between(target, next_tile);
            if (dist < min_distance) {
                min_distance = dist;
                best_dir = dir;
            }
        }
    }

    return best_dir;
}

bool is_in_house(entity_t *g) {
    return g->state == STATE_IN_HOUSE || g->state == STATE_LEAVING_HOUSE;
}

static void update_ghost(entity_t *g) {
    update_ghost_frame(g);

    if (g->state == STATE_IN_HOUSE) {
        g->bounce += BOUNCE_SPEED;
        g->pos.y = g->tile.y * TILE + sinf(g->bounce) * BOUNCE_AMPLITUDE;
        g->dir = cosf(g->bounce) > 0 ? DIR_SOUTH : DIR_NORTH;

        if (g->leave()) {
            g->state = STATE_LEAVING_HOUSE;
            g->dir = DIR_NORTH;
            g->target = (Vector2){13, 11};
        }

        return;
    }

    if (g->state == STATE_LEAVING_HOUSE) {
        //printf("Ghost leaving house at %f, %f\n", g->tile.x, g->tile.y);
        if ((g->tile.x == 13 || g->tile.x == 14) && g->tile.y == 11) {
            g->state = world.game.ghost_state;
            world.game.paused = true;
            return;
        }
    }

    if (g->pixels_moved >= TILE) {
        set_next_tile(g, g->dir);
        g->pixels_moved = 0;
    }

    Vector2 target;
    if (g->state == STATE_CHASE) {
        target = g->chase();
    } else {
        target = g->scatter();
    }

    g->target = target;

    const dir_t current_dir = g->dir;

    if (is_in_tunnel(g->tile)) {
        if (g->tile.x < 0 && g->dir == DIR_WEST) {
            g->tile.x = GAME_WIDTH - 1;
        } else if (g->tile.x >= GAME_WIDTH-1 && g->dir == DIR_EAST) {
            g->tile.x = 0;
        }
    } else {
        g->dir = choose_ghost_dir(g, target);
    }

    float speed = ghost_speed(g);

    if (current_dir == g->dir) {
         g->pixels_moved_in_dir += speed;
    } else {
         g->pixels_moved_in_dir = speed;
    }

    move_entity(g, speed);
}

static void update_fright(entity_t *g) {
    if (g->state != STATE_FRIGHTENED) {
        return;
    }

    const double dt = world.game.fright_time - GetTime();
    if (dt < 0) {
        g->state = STATE_CHASE;
        g->sprite_type = SPRITE_ENTITY;
        return;
    }

    if (dt > 2.0) {
        g->sprite_type = SPRITE_BLUE;
        return;
    }

    const int n = (int)round(dt * 200) % 100;
    if (n > 49) {
        g->sprite_type = SPRITE_BLUE;
    } else {
        g->sprite_type = SPRITE_WHITE;
    }
}

void update_ghosts() {
    for (int i = 0; i < GHOST_COUNT; i+=1) {
        entity_t *g = &world.ghosts[i];
        update_fright(g);
        update_ghost(g);
    }
}