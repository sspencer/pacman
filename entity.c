//
// Created by Steve Spencer on 12/8/25.
//

#include "entity.h"
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

static void update_player_frame(entity_t *entity) {
    const int frames_per_state = 5;
    const int cycle_length = 4 * frames_per_state;
    const int frame_in_cycle = entity->frame_count % cycle_length;

    if (frame_in_cycle < frames_per_state) {
        entity->frame = 2; // closed
    } else if (frame_in_cycle < 2 * frames_per_state) {
        entity->frame = 1; // half open
    } else if (frame_in_cycle < 3 * frames_per_state) {
        entity->frame = 0; // fully open
    } else {
        entity->frame = 1; // half open (returning
    }

    entity->frame_count++;
    if (entity->frame_count > cycle_length) {
        entity->frame_count = 0;
    }
}

void update_player(entity_t *entity) {
    update_player_frame(entity);
}