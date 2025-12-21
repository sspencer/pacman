//
// Created by Steve Spencer on 12/8/25.
//

#include "pacman.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "world.h"

#define POWER_EAT_PAUSE 3
#define DOT_EAT_PAUSE 1

void init_pacman(entity_t *entity) {
    const float tile_x = 13, tile_y = 23;
    entity->pos.x = tile_x * TILE;
    entity->pos.y = tile_y * TILE;
    entity->tile.x = tile_x;
    entity->tile.y = tile_y;

    entity->sprite[DIR_EAST] = (Vector2){456, 0};
    entity->sprite[DIR_WEST] = (Vector2){456, 16};
    entity->sprite[DIR_NORTH] = (Vector2){456, 32};
    entity->sprite[DIR_SOUTH] = (Vector2){456, 48};

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

    // Determine the correct anchor tile for collision checks based on the axis
    // of intended movement. When turning perpendicular, we should base the
    // check on the rounded coordinate along the perpendicular axis instead of
    // the source tile, otherwise valid turns at junctions can be rejected.
    int anchor_tx = (int)p->tile.x;
    int anchor_ty = (int)p->tile.y;

    if (dir == DIR_NORTH || dir == DIR_SOUTH) {
        // Turning/moving vertically: anchor column should follow our current X position
        // rounded to the nearest tile column.
        int col = (int)lroundf(p->pos.x / (float)TILE);
        if (col < 0) col = 0; else if (col >= GAME_WIDTH) col = GAME_WIDTH - 1;
        anchor_tx = col;
    } else { // DIR_EAST or DIR_WEST
        // Turning/moving horizontally: anchor row should follow our current Y position
        // rounded to the nearest tile row.
        int row = (int)lroundf(p->pos.y / (float)TILE);
        if (row < 0) row = 0; else if (row >= GAME_HEIGHT) row = GAME_HEIGHT - 1;
        anchor_ty = row;
    }

    Vector2 anchor = (Vector2){(float)anchor_tx, (float)anchor_ty};
    // Compute the next tile from the corrected anchor.
    switch (dir) {
        case DIR_NORTH: anchor.y -= 1; break;
        case DIR_SOUTH: anchor.y += 1; break;
        case DIR_EAST:  anchor.x += 1; break;
        case DIR_WEST:  anchor.x -= 1; break;
        default: break;
    }

    if (anchor.x < 0 || anchor.x >= GAME_WIDTH || anchor.y < 0 || anchor.y >= GAME_HEIGHT) return false;

    return game->maze[(int)anchor.y][(int)anchor.x] != TILE_WALL;
}

static inline bool dirs_opposite(dir_t a, dir_t b) {
    return (a == DIR_EAST && b == DIR_WEST) ||
           (a == DIR_WEST && b == DIR_EAST) ||
           (a == DIR_NORTH && b == DIR_SOUTH) ||
           (a == DIR_SOUTH && b == DIR_NORTH);
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
    }

    // Decide next move. Allow smooth reversal on the same axis without a visual gap
    if (can_pacman_move(p, p->next_dir)) {
        if (dirs_opposite(p->dir, p->next_dir)) {
            // Remap anchor tile so the world position remains continuous after reversing.
            // Also mirror pixels_moved so world position does not jump.
            // If we were blocked on the old direction, don't remap; just reset progress.
            if (can_pacman_move(p, p->dir)) {
                switch (p->dir) {
                    case DIR_EAST:  // reversing to WEST -> anchor one tile to the east
                        if (p->tile.x + 1 < GAME_WIDTH) p->tile.x += 1;
                        break;
                    case DIR_WEST:  // reversing to EAST -> anchor one tile to the west
                        if (p->tile.x - 1 >= 0) p->tile.x -= 1;
                        break;
                    case DIR_NORTH: // reversing to SOUTH -> anchor one tile above
                        if (p->tile.y - 1 >= 0) p->tile.y -= 1;
                        break;
                    case DIR_SOUTH: // reversing to NORTH -> anchor one tile below
                        if (p->tile.y + 1 < GAME_HEIGHT) p->tile.y += 1;
                        break;
                    default: break;
                }
                // Mirror the progress within the tile: m' = TILE - m
                if (p->pixels_moved > 0.0f && p->pixels_moved < TILE) {
                    p->pixels_moved = TILE - p->pixels_moved;
                }
            } else {
                // Blocked on old axis: start fresh in the opposite direction without any nudge.
                p->pixels_moved = 0.0f;
            }
        } else if (p->dir != p->next_dir) {
            // Turning perpendicular. Keep Pacman centered by aligning the anchor on the
            // perpendicular axis to the current position, and start the new axis from
            // the tile center (reset intra-tile progress).
            if (p->next_dir == DIR_NORTH || p->next_dir == DIR_SOUTH) {
                int new_tx = (int)lroundf(p->pos.x / (float)TILE);
                if (new_tx < 0) new_tx = 0; else if (new_tx >= GAME_WIDTH) new_tx = GAME_WIDTH - 1;
                p->tile.x = (float)new_tx;
            } else { // turning to EAST/WEST
                int new_ty = (int)lroundf(p->pos.y / (float)TILE);
                if (new_ty < 0) new_ty = 0; else if (new_ty >= GAME_HEIGHT) new_ty = GAME_HEIGHT - 1;
                p->tile.y = (float)new_ty;
            }
            // Always reset progress when switching axes so the new movement starts
            // from the tile center along the new axis, preventing off-center turns.
            p->pixels_moved = 0.0f;
        }
        p->dir = p->next_dir;
        can_move = true;
    } else if (can_pacman_move(p, p->dir)) {
        can_move = true;
    }

    // TODO remove else clause if these conditions are always true
    if (p->tile.x >= 0 && p->tile.x < GAME_WIDTH && p->tile.y >= 0 && p->tile.y < GAME_HEIGHT) {
        const tile_t tile = game->maze[(int)p->tile.y][(int)p->tile.x];
        if (tile == TILE_DOT) {
            game->maze[(int)p->tile.y][(int)p->tile.x] = TILE_EMPTY;
            game->dots_eaten++;
            game->score += 10;
            p->frames_to_pause = DOT_EAT_PAUSE;
        } else if (tile == TILE_POWER) {
            game->maze[(int)p->tile.y][(int)p->tile.x] = TILE_EMPTY;
            game->score += 50;
            set_ghost_state(STATE_FRIGHTENED);
            if (p->frames_to_pause == 0) { // don't overwrite power eat
                p->frames_to_pause = POWER_EAT_PAUSE;
            }
        }
    } else {
        printf("Pacman went off the map! tx=%f, ty=%f\n", p->tile.x, p->tile.y);
        exit(1);
    }

    if (can_move) {
        move_entity(p, pacman_speed());
    }
}
