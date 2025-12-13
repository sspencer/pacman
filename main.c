#include <math.h>
#include <stdio.h>

#include "raylib.h"
#include "game.h"
#include "maze.h"
#include "draw.h"
#include "pacman.h"
#include "ghost.h"
#include "world.h"

world_t world;
float screen_zoom = 1.0f;

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
    screen_zoom = floorf(mon.y / (TILE * SCREEN_HEIGHT));

    InitWindow(SCREEN_WIDTH * TILE * screen_zoom, SCREEN_HEIGHT * TILE * screen_zoom, "Ms. Pacman");
    SetWindowPosition((int)mon.x - (int)(SCREEN_WIDTH * TILE * screen_zoom), 0);
    SetTraceLogLevel(LOG_WARNING);
    SetTargetFPS(60);

    world.game_texture = LoadTexture("assets/game.png");
    world.game_image = LoadImageFromTexture(world.game_texture);
    world.game.level = 0;
    world.game.paused = false;
    world.game.score = 0;
    world.game.dots_eaten = 0;
    world.game.debug = false;

    map_maze(&world.game);

    init_pacman(&world.pacman);
    init_ghost(&world.ghosts[GHOST_BLINKY], blinky_data());
    init_ghost(&world.ghosts[GHOST_INKY], inky_data());
    init_ghost(&world.ghosts[GHOST_PINKY], pinky_data());
    init_ghost(&world.ghosts[GHOST_SUE], sue_data());

    const Shader shader = chroma_shader();

    world.game.level_time = 0.0; // TODO reset for every level

    double last_frame_time = GetTime();

    while (!WindowShouldClose()) {
        const double current_time = GetTime();
        if (!world.game.paused) world.game.level_time += (current_time - last_frame_time);
        last_frame_time = current_time;

        update_world();

        BeginDrawing();
        ClearBackground(BLACK);
        draw_world(screen_zoom, shader);
        EndDrawing();
    }

    UnloadImage(world.game_image);
    UnloadTexture(world.game_texture);
    CloseWindow();

    return 0;
}

