#include <stdio.h>

#include "raylib.h"
#include "game.h"
#include "maze.h"
#include "draw.h"

static void update_game(game_t *game, Image *image) {
    if (IsKeyPressed(KEY_N)) {
        game->level = (game->level + 1) % 5;
        map_maze(game, image);
    }
}


int main(void) {
    InitWindow(SCREEN_WIDTH * PIXEL, SCREEN_HEIGHT * PIXEL, "Ms. Pacman");
    SetWindowPosition(GetMonitorWidth(0) - (SCREEN_WIDTH * PIXEL), 0);
    SetTraceLogLevel(LOG_WARNING);
    SetTargetFPS(60);

    Texture2D game_texture = LoadTexture("assets/game.png");
    Image game_image = LoadImageFromTexture(game_texture);

    game_t game = {.level=0};
    map_maze(&game, &game_image);

    if (DEBUG) {
        printf("%d x %d\n", game_texture.width, game_texture.height);
        debug_maze(&game);
    }

    // offset camera to allow for score on top
    static Camera2D camera = {
        .offset = {0, TOP_PADDING * PIXEL},
        .target = {0, 0},
        .rotation = 0.0f,
        .zoom = 1.0f
    };

    while (!WindowShouldClose()) {
        update_game(&game, &game_image);

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        draw_maze(&game, game_texture);
        EndMode2D();
        EndDrawing();
    }

    UnloadImage(game_image);
    UnloadTexture(game_texture);
    CloseWindow();

    return 0;
}