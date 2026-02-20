#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>

#include "game.h"
#include "ghost.h"
#include "maze.h"
#include "player.h"
#include "prng.h"

void load_chroma_shader(void);

// globals
Game game;

int main(void) {
    // Seed for reproducibility (simulating fixed ROM patterns)
    srand(42);

    game.screen_zoom = 4.0f;

    InitWindow(SCREEN_WIDTH * TILE * game.screen_zoom, SCREEN_HEIGHT * TILE * game.screen_zoom, "Ms. Pacman");
    SetTraceLogLevel(LOG_WARNING);
    SetTargetFPS(62);

    event_stack_init();
    init_game();
    init_prng();
    init_level(1);
    load_chroma_shader();
    init_pacman(&game.player);
    init_blinky(&game.ghosts[BLINKY]);
    init_inky(&game.ghosts[INKY]);
    init_pinky(&game.ghosts[PINKY]);
    init_sue(&game.ghosts[SUE]);
    init_maze(game.level);
    // need a init_level func

    while (!WindowShouldClose()) {
        update_game();

        BeginDrawing();
        ClearBackground(BLACK);
        draw_game();
        EndDrawing();
    }

    UnloadTexture(game.sprite_texture);
    UnloadTexture(game.font_texture);
    UnloadShader(game.chroma_shader);
    CloseWindow();

    return 0;
}

void load_chroma_shader(void) {
    game.chroma_shader = LoadShader(NULL, "assets/chroma_key.fs");
    constexpr float key_color[3] = {0.0f, 0.0f, 0.0f};
    constexpr float threshold = 0.05f;
    const int key_color_loc = GetShaderLocation(game.chroma_shader, "keyColor");
    const int threshold_loc = GetShaderLocation(game.chroma_shader, "threshold");

    SetShaderValue(game.chroma_shader, key_color_loc, key_color, SHADER_UNIFORM_VEC3);
    SetShaderValue(game.chroma_shader, threshold_loc, &threshold, SHADER_UNIFORM_FLOAT);
}
