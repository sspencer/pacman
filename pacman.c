//
// Created by Steve Spencer on 12/8/25.
//

#include "pacman.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"

#define POWER_EAT_PAUSE 3
#define DOT_EAT_PAUSE 1

void init_pacman(entity_t *entity) {
    const float tile_x = 13, tile_y = 23;
    entity->pos.x = tile_x * TILE;
    entity->pos.y = tile_y * TILE;
    entity->tile.x = tile_x;
    entity->tile.y = tile_y;

    entity->sprite_x[DIR_EAST] = 456;
    entity->sprite_y[DIR_EAST] = 0;
    entity->sprite_x[DIR_WEST] = 456;
    entity->sprite_y[DIR_WEST] = 16;
    entity->sprite_x[DIR_NORTH] = 456;
    entity->sprite_y[DIR_NORTH] = 32;
    entity->sprite_x[DIR_SOUTH] = 456;
    entity->sprite_y[DIR_SOUTH] = 48;

    entity->dir = DIR_WEST;
    entity->next_dir = DIR_WEST;

    entity->frame_count = 0;
    entity->frame_index = 0;
}

static void update_pacman_frame(entity_t *p) {
    const int frames_per_state = 5;
    const int cycle_length = 4 * frames_per_state;
    const int frame_in_cycle = p->frame_count % cycle_length;

    if (frame_in_cycle < frames_per_state) {
        p->frame_index = 2; // closed
    } else if (frame_in_cycle < 2 * frames_per_state) {
        p->frame_index = 1; // half open
    } else if (frame_in_cycle < 3 * frames_per_state) {
        p->frame_index = 0; // fully open
    } else {
        p->frame_index = 1; // half open (returning
    }

    p->frame_count++;
    if (p->frame_count > cycle_length) {
        p->frame_count = 0;
    }
}

static float pacman_speed() {
    const int level = world.game.level;
    float speed;
    // TODO too simple, needs updating
    if (level <= 1) {
        speed = 88.0f;
    } else if (level < 5) {
        speed = 96.8f;
    } else {
        speed = 105.6f;
     }

    return (1.023f * speed) / 60.0f;
}

static bool can_pacman_move(entity_t *p, dir_t dir) {
    game_t *game = &world.game;

    const Vector2 next_tile = get_next_tile(p, dir);
    if (next_tile.x < 0 || next_tile.x >= GAME_WIDTH || next_tile.y < 0 || next_tile.y >= GAME_HEIGHT) return false;

    return game->maze[(int)next_tile.y][(int)next_tile.x] != TILE_WALL;
}

void update_pacman() {
    game_t *game = &world.game;
    entity_t *p = &world.pacman;

    update_pacman_frame(p);

    // eating dots makes pacman drop frames
    if (p->frames_to_pause > 0) {
        p->frames_to_pause--;
        return;
    }

    bool can_move = false;

    if (p->pixels_moved >= TILE) {
        set_next_tile(p, p->dir);
        p->pixels_moved = 0;
        p->eating_dot = false;
    }

    // At the new intersection, decide the next move
    if (can_pacman_move(p, p->next_dir)) {
        p->dir = p->next_dir;
        can_move = true;
        // else if in the tunnel
    } else if (can_pacman_move(p, p->dir)) {
        can_move = true;
    }


    if (!p->eating_dot) {
        if (p->tile.x >= 0 && p->tile.x < GAME_WIDTH && p->tile.y >= 0 && p->tile.y < GAME_HEIGHT) {
            tile_t tile = game->maze[(int)p->tile.y][(int)p->tile.x];
            if (tile == TILE_DOT) {
                game->maze[(int)p->tile.y][(int)p->tile.x] = TILE_EMPTY;
                p->eating_dot = true;
                game->dots_eaten++;
                game->score += 10;
                p->frames_to_pause = DOT_EAT_PAUSE;
            } else if (tile == TILE_POWER) {
                game->maze[(int)p->tile.y][(int)p->tile.x] = TILE_EMPTY;
                p->eating_dot = true;
                game->score += 50;
                if (p->frames_to_pause == 0) { // don't overwrite power eat
                    p->frames_to_pause = POWER_EAT_PAUSE;
                }
            }
        } else {
            printf("Pacman went off the map! tx=%f, ty=%f\n", p->tile.x, p->tile.y);
            exit(1);
        }
    }
    // if (vel.x == 0 && vel.y == 0) {
    //     vel = velocity[p->next_dir];
    //     if (can_player_move(vel)) {
    //         p->dir = p->next_dir;
    //         vel = velocity[p->dir];
    //     }
    // }

    if (can_move) {
        move_entity(p, pacman_speed());
    }
}
