#include <math.h>
#include <stdio.h>

#include "raylib.h"
#include "game.h"
#include "maze.h"
#include "draw.h"
#include "pacman.h"
#include "ghost.h"

// Define global world declared in game.h
world_t world;
Vector2 velocity[DIR_COUNT];
float zoom = 1.0f;

static void update_world(void) {
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

static void draw_world(void) {
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

    BeginMode2D(sprite_camera);
    draw_ghosts();
    draw_pacman();
    EndMode2D();
}


Shader chroma_shader() {
    const Shader shader = LoadShader(NULL, "assets/chroma_key.fs");
    const float key_color[3] = {0.0f, 0.0f, 0.0f};
    const float threshold = 0.05f;
    const int key_color_loc = GetShaderLocation(shader, "keyColor");
    const int threshold_loc = GetShaderLocation(shader, "threshold");

    SetShaderValue(shader, key_color_loc, key_color, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, threshold_loc, &threshold, SHADER_UNIFORM_FLOAT);

    return shader;
}

static Vector2 monitor_size() {
    // Need an initial window to reliably get the screen size so an appropriate zoom level may be set
    // Make the initial window hidden so it won't flash on screen
    SetConfigFlags(FLAG_WINDOW_HIDDEN);

    // Create a tiny hidden window which forces raylib/GLFW to initialise
    InitWindow(1, 1, "init-hidden");

    // Now monitor queries are reliable
    const int currentMon = GetCurrentMonitor();   // should be valid now
    const int sh = GetMonitorHeight(currentMon);
    const int sw = GetMonitorWidth(currentMon);

    CloseWindow();
    ClearWindowState(FLAG_WINDOW_HIDDEN);

    // TODO remove this magic
    // subtracting magic value from height so zoom isn't too large on my dev machine
    return (Vector2){(float)sw, (float)(sh-350)};
}

int main(void) {
    const Vector2 mon = monitor_size();
    zoom = floorf(mon.y / (TILE * SCREEN_HEIGHT));

    InitWindow(SCREEN_WIDTH * TILE * zoom, SCREEN_HEIGHT * TILE * zoom, "Ms. Pacman");
    SetWindowPosition((int)mon.x - (int)(SCREEN_WIDTH * TILE * zoom), 0);
    SetTraceLogLevel(LOG_WARNING);
    SetTargetFPS(60);

    velocity[DIR_NORTH] = (Vector2){0, -1};
    velocity[DIR_SOUTH] = (Vector2){0, 1};
    velocity[DIR_EAST] = (Vector2){1, 0};
    velocity[DIR_WEST] = (Vector2){-1, 0};

    world.game_texture = LoadTexture("assets/game.png");
    world.game_image = LoadImageFromTexture(world.game_texture);

    world.game.level = 0;
    world.game.paused = false;
    world.game.score = 0;
    world.game.dots_eaten = 0;
    map_maze(&world.game);

    init_pacman(&world.pacman);
    init_ghost(&world.ghosts[GHOST_BLINKY], blinky_data());
    init_ghost(&world.ghosts[GHOST_INKY], inky_data());
    init_ghost(&world.ghosts[GHOST_PINKY], pinky_data());
    init_ghost(&world.ghosts[GHOST_SUE], sue_data());

    Shader shader = chroma_shader();

    while (!WindowShouldClose()) {
        update_world();

        BeginDrawing();
        ClearBackground(BLACK);
        BeginShaderMode(shader);
        draw_world();
        EndShaderMode();
        EndDrawing();
    }

    UnloadImage(world.game_image);
    UnloadTexture(world.game_texture);
    CloseWindow();

    return 0;
}

