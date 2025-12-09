//
// Created by Steve Spencer on 12/8/25.
//

#include "entity.h"

#include <math.h>

#include "game.h"
/*

dir_t dir;
dir_t next_dir;
Vector2 vel;
Vector2 next_vel;

Vector2 pixel;
float pixels_moved;
float width;
float height;
int frame_count;
int frame;
bool eaten;
double speed_time;
float speed_pixels;
union {
player_data_t pacman;
ghost_data_t ghost;
} entity_data;
*/

static Vector2 velocity(dir_t dir) {
    switch (dir) {
        case DIR_NORTH: return (Vector2){.x = 0, .y = -1};
        case DIR_SOUTH: return (Vector2){.x = 0, .y = 1};
        case DIR_EAST: return (Vector2){.x = 1, .y = 0};
        case DIR_WEST: return (Vector2){.x = -1, .y = 0};
        default: return (Vector2){.x = 0, .y = 0};
    }
}

void init_player(entity_t *entity) {
    float start_x = 13;
    float start_y = 23;

    dir_t start_dir = DIR_WEST;
    entity->type = TYPE_PLAYER;
    entity->name = NAME_PLAYER;
    entity->tile = (Vector2){.x = start_x, .y = start_y}; // starting tile
    entity->pixel =  (Vector2){start_x * PIXEL, start_y * PIXEL};
    entity->sprite[DIR_NORTH] = (Vector2){.x = 456, .y = 32};
    entity->sprite[DIR_SOUTH] = (Vector2){.x = 456, .y = 48};
    entity->sprite[DIR_EAST] = (Vector2){.x = 456, .y = 0};
    entity->sprite[DIR_WEST] = (Vector2){.x = 456, .y = 16};
    entity->dir = start_dir;
    entity->next_dir = start_dir;
    entity->vel = velocity(start_dir);
    entity->next_vel = velocity(start_dir);
    entity->entity_data.player.score = 0;
    entity->width = 16;
    entity->height = 16;
    entity->frame_count = 0;
    entity->frame = 0;
}

static void update_player_frame(entity_t *p) {
    const int frames_per_state = 5;
    const int cycle_length = 4 * frames_per_state;
    const int frame_in_cycle = p->frame_count % cycle_length;

    if (frame_in_cycle < frames_per_state) {
        p->frame = 2; // closed
    } else if (frame_in_cycle < 2 * frames_per_state) {
        p->frame = 1; // half open
    } else if (frame_in_cycle < 3 * frames_per_state) {
        p->frame = 0; // fully open
    } else {
        p->frame = 1; // half open (returning
    }

    p->frame_count++;
    if (p->frame_count > cycle_length) {
        p->frame_count = 0;
    }


}

static float calculate_player_speed(int level) {
    float speed;

    if (level <= 1) {
        speed = 88.0f;
    } else if (level < 5) {
        speed = 96.8f;
    } else {
        speed = 105.6f;
     }

    return (1.023f * speed) / 60.0f;
}

static bool can_player_move(game_t *game, entity_t *p, Vector2 dir) {
    if (dir.x == 0 && dir.y == 0) return false;
    const Vector2 next_tile = (Vector2){p->tile.x + dir.x, p->tile.y + dir.y};
    if (next_tile.x < 0 || next_tile.x >= GAME_WIDTH || next_tile.y < 0 || next_tile.y >= GAME_HEIGHT) return false;

    return game->maze[(int)next_tile.y][(int)next_tile.x] != TILE_WALL;
}

static void move_entity(entity_t *e, float speed) {
    if (e->vel.x != 0 || e->vel.y != 0) {
        e->pixels_moved += speed;

        float clamped_pixels_moved = fminf(e->pixels_moved, SIZE);
        float visual_offset_x = e->vel.x * clamped_pixels_moved;
        float visual_offset_y = e->vel.y * clamped_pixels_moved;

        //. tile size or zoom
        e->pixel.x = (e->tile.x * SIZE + visual_offset_x - SIZE/2.0f) * ZOOM;
        e->pixel.y = (e->tile.y * SIZE + visual_offset_y - SIZE/2.0f) * ZOOM;
    }
}

void update_player(game_t *game, entity_t *p) {
    update_player_frame(p);

    if (p->pixels_moved >= SIZE) {
        p->tile.x += p->vel.x;
        p->tile.y += p->vel.y;
        p->pixels_moved = 0;
        p->entity_data.player.eating_dot = false;
    }

    // At the new intersection, decide the next move
    if (can_player_move(game, p, p->next_vel)) {
        p->dir = p->next_dir;
        p->vel = p->next_vel;
        // else if in the tunnel
    } else {
        p->vel.x = 0;
        p->vel.y = 0;
    }

    // handle starting from a standstill
    if (p->vel.x == 0 && p->vel.y == 0) {
        if (can_player_move(game, p, p->next_vel)) {
            p->dir = p->next_dir;
            p->vel = p->next_vel;
        }
    }

    float speed = calculate_player_speed(game->level);
    move_entity(p, speed);
    p->speed_time += speed;
}
