//
// Created by Steve Spencer on 12/12/25.
//

#include "world.h"
#include "game.h"
#include "ghost.h"
#include "pacman.h"
#include "maze.h"
#include "draw.h"

void update_world(void) {
    if (world.game.paused) return;

    if (IsKeyPressed(KEY_P)) {
        world.game.paused = !world.game.paused;
        return;
    }

    if (IsKeyPressed(KEY_N)) {
        world.game.level = (world.game.level + 1) % 5;
        map_maze(&world.game);
    }

    if (IsKeyPressed(KEY_LEFT)) {
        world.pacman.next_dir = DIR_WEST;
    }

    if (IsKeyPressed(KEY_RIGHT)) {
        world.pacman.next_dir = DIR_EAST;
    }

    if (IsKeyPressed(KEY_UP)) {
        world.pacman.next_dir = DIR_NORTH;
    }

    if (IsKeyPressed(KEY_DOWN)) {
        world.pacman.next_dir = DIR_SOUTH;
    }

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
    EndMode2D();

    BeginShaderMode(shader); // shader makes BLACK transparent
    BeginMode2D(sprite_camera);
    draw_ghosts();
    draw_pacman();
    EndMode2D();
    EndShaderMode();
}


