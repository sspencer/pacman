#include <stdio.h>

#include "raylib.h"
#include "game.h"
#include "maze.h"
#include "draw.h"
#include "entity.h"



// Define global world declared in game.h
world_t world;

static void update_world(void) {
    static Vector2 north_vel = {0, -1};
    static Vector2 south_vel = {0, 1};
    static Vector2 east_vel = {1, 0};
    static Vector2 west_vel = {-1, 0};

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
        world.player.next_dir = DIR_WEST;
        world.player.next_vel = west_vel;//(Vector2){-1, 0};
    }

    if (IsKeyPressed(KEY_RIGHT)) {
        world.player.next_dir = DIR_EAST;
        world.player.next_vel = east_vel;// (Vector2){1, 0};
    }

    if (IsKeyPressed(KEY_UP)) {
        world.player.next_dir = DIR_NORTH;
        world.player.next_vel = north_vel;//(Vector2){0, -1};
    }

    if (IsKeyPressed(KEY_DOWN)) {
        world.player.next_dir = DIR_SOUTH;
        world.player.next_vel = south_vel;//(Vector2){0, 1};
    }

    update_player(&world.game, &world.player);
}

static void draw_world(void) {
    draw_maze();
    draw_player();
}


Shader chroma_shader() {
    Shader shader = LoadShader(NULL, "assets/chroma_key.fs");
    const float key_color[3] = {0.0f, 0.0f, 0.0f};
    const float threshold = 0.05f;
    const int key_color_loc = GetShaderLocation(shader, "keyColor");
    const int threshold_loc = GetShaderLocation(shader, "threshold");

    SetShaderValue(shader, key_color_loc, key_color, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, threshold_loc, &threshold, SHADER_UNIFORM_FLOAT);

    return shader;
}

int main(void) {
    InitWindow(SCREEN_WIDTH * PIXEL, SCREEN_HEIGHT * PIXEL, "Ms. Pacman");
    SetWindowPosition(GetMonitorWidth(0) - (SCREEN_WIDTH * PIXEL), 0);
    SetTraceLogLevel(LOG_WARNING);
    SetTargetFPS(60);

    world.game_texture = LoadTexture("assets/game.png");
    world.game_image = LoadImageFromTexture(world.game_texture);

    world.game.level = 0;
    world.game.paused = false;
    map_maze(&world.game);

    world.player = (entity_t){};
    init_player(&world.player);

    if (DEBUG) {
        printf("%d x %d\n", world.game_texture.width, world.game_texture.height);
        debug_maze(&world.game);
    }

    // offset camera to allow for score on top
    static Camera2D camera = {
        .offset = {0, TOP_PADDING * PIXEL},
        .target = {0, 0},
        .rotation = 0.0f,
        .zoom = 1.0f
    };

    Shader shader = chroma_shader();

    while (!WindowShouldClose()) {
        update_world();

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        BeginShaderMode(shader);
        draw_world();
        EndShaderMode();
        EndMode2D();
        EndDrawing();
    }

    UnloadImage(world.game_image);
    UnloadTexture(world.game_texture);
    CloseWindow();

    return 0;
}

