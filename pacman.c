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
    entity->tx = 13;
    entity->ty = 23;
    entity->x = (float)entity->tx * TILE;
    entity->y = (float)entity->ty * TILE;

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

    entity->frame_count = 3;
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

static float calculate_pacman_speed(int level) {
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

static bool can_pacman_move(Vector2 dir) {
    game_t *game = &world.game;
    entity_t *p = &world.pacman;

    if (dir.x == 0 && dir.y == 0) return false;
    const Vector2 next_tile = (Vector2){p->tx + dir.x, p->ty + dir.y};
    if (next_tile.x < 0 || next_tile.x >= GAME_WIDTH || next_tile.y < 0 || next_tile.y >= GAME_HEIGHT) return false;

    return game->maze[(int)next_tile.y][(int)next_tile.x] != TILE_WALL;
}

static void move_pacman(Vector2 vel, float speed) {
    entity_t *e = &world.pacman;
    if (vel.x != 0 || vel.y != 0) {
        e->pixels_moved += speed;

        const float clamp = fminf(e->pixels_moved, TILE);

        e->x = ((float)e->tx * TILE + (vel.x * clamp));
        e->y = ((float)e->ty * TILE + (vel.y * clamp));
    }
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

    Vector2 vel = (Vector2){0, 0};

    if (p->pixels_moved >= TILE) {
        p->tx += (int)velocity[p->dir].x;
        p->ty += (int)velocity[p->dir].y;
        p->pixels_moved = 0;
        p->eating_dot = false;
    }

    // At the new intersection, decide the next move
    if (can_pacman_move(velocity[p->next_dir])) {
        p->dir = p->next_dir;
        vel = velocity[p->dir];
        // else if in the tunnel
    } else if (can_pacman_move(velocity[p->dir])) {
        vel = velocity[p->dir];
    }


    if (!p->eating_dot) {
        if (p->tx >= 0 && p->tx < GAME_WIDTH && p->ty >= 0 && p->ty < GAME_HEIGHT) {
            tile_t tile = game->maze[p->ty][p->tx];
            if (tile == TILE_DOT) {
                game->maze[p->ty][p->tx] = TILE_EMPTY;
                p->eating_dot = true;
                game->dots_eaten++;
                game->score += 10;
                p->frames_to_pause = DOT_EAT_PAUSE;
            } else if (tile == TILE_POWER) {
                game->maze[p->ty][p->tx] = TILE_EMPTY;
                p->eating_dot = true;
                game->score += 50;
                if (p->frames_to_pause == 0) { // don't overwrite power eat
                    p->frames_to_pause = POWER_EAT_PAUSE;
                }
            }
        } else {
            printf("Pacman went off the map! tx=%d, ty=%d\n", p->tx, p->ty);
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

    float speed = calculate_pacman_speed(game->level);
    move_pacman(vel, speed);
}
