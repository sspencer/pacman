//
// Created by Steve Spencer on 12/12/25.
//

#include "world.h"

#include <stdio.h>

#include "game.h"
#include "ghost.h"
#include "pacman.h"
#include "maze.h"
#include "draw.h"

// Set ghost state based on level timer.
// Does this change based on level??
//
// a. Scatter (7s),
// b. Chase (20s)
// c. Scatter (7s)
// d. Chase (20s)
// e. Scatter (5s),
// f. Chase (20s),
// g. Scatter (5s),
// h. Chase indefinitely.
static void update_ghost_state(void) {
    game_t *game = &world.game;

    const int elapsed = (int)game->level_time;

    // Phase thresholds are cumulative seconds from level start.
    // After the last threshold, ghosts remain in CHASE indefinitely.
    static const struct { int end_time; ghost_state_t state; } phases[] = {
        {  7, GHOST_SCATTER },
        { 27, GHOST_CHASE   },
        { 34, GHOST_SCATTER },
        { 54, GHOST_CHASE   },
        { 61, GHOST_SCATTER },
        { 81, GHOST_CHASE   },
        { 86, GHOST_SCATTER },
    };

    ghost_state_t desired = GHOST_CHASE; // default after final phase
    for (size_t i = 0; i < sizeof(phases)/sizeof(phases[0]); ++i) {
        if (elapsed < phases[i].end_time) { desired = phases[i].state; break; }
    }

    for (int i = 0; i < GHOST_COUNT; i++) {
        entity_t *e = &world.ghosts[i];
        if (e->state == GHOST_SCATTER || e->state == GHOST_CHASE) {
            e->state = desired;
        }
    }
}

void update_world(void) {
    if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_SPACE)) world.game.paused = !world.game.paused;
    if (IsKeyPressed(KEY_D)) world.game.debug = !world.game.debug;
    if (IsKeyPressed(KEY_N)) {
        world.game.level = (world.game.level + 1) % 5;
        map_maze(&world.game);
    }

    if (world.game.paused) return;

    if (IsKeyPressed(KEY_LEFT)) world.pacman.next_dir = DIR_WEST;
    if (IsKeyPressed(KEY_RIGHT)) world.pacman.next_dir = DIR_EAST;
    if (IsKeyPressed(KEY_UP)) world.pacman.next_dir = DIR_NORTH;
    if (IsKeyPressed(KEY_DOWN)) world.pacman.next_dir = DIR_SOUTH;
    if (IsKeyPressed(KEY_C)) {
        for (int i = 0; i < GHOST_COUNT; i++) {
            world.ghosts[i].state = GHOST_CHASE;
        }
    }
    if (IsKeyPressed(KEY_S)) {
        for (int i = 0; i < GHOST_COUNT; i++) {
            world.ghosts[i].state = GHOST_SCATTER;
        }
    }

    update_ghost_state(); // change between SCATTER & CHASE
    update_pacman();
    update_ghosts();
}

void draw_world(float zoom, Shader shader) {
    const Camera2D maze_camera = {
        .offset = {0, TOP_PADDING * TILE * zoom},
        .target = {0, 0},
        .rotation = 0.0f,
        .zoom = zoom
    };

    const float nudge = TILE/2.0f;
    const Camera2D sprite_camera = {
        .offset = {0, TOP_PADDING * TILE * zoom},
        .target = {nudge, nudge},
        .rotation = 0.0f,
        .zoom = zoom
    };

    BeginMode2D(maze_camera);
    draw_maze();
    //if (world.game.debug) draw_checkerboard();
    EndMode2D();

    BeginShaderMode(shader); // shader makes BLACK transparent
    BeginMode2D(sprite_camera);
    draw_ghosts();
    draw_pacman();
    EndMode2D();
    EndShaderMode();
}


