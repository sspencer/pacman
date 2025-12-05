#include <stdio.h>

#include "raylib.h"
#include "game.h"
#include "maze.h"


int main(void) {
    InitWindow(SCREEN_WIDTH * PIXEL, SCREEN_HEIGHT * PIXEL, "Ms. Pacman");
    SetWindowPosition(GetMonitorWidth(0) - (SCREEN_WIDTH * PIXEL), 0);
    SetTraceLogLevel(LOG_WARNING);
    SetTargetFPS(60);
    Texture2D gameTexture = LoadTexture("assets/game.png");
    Image gameImage = LoadImageFromTexture(gameTexture);
    struct Game game = {0};
    game.level = 0;

    createMap(&game, &gameImage);
    printf("%d x %d\n", gameTexture.width, gameTexture.height);

    printMaze(&game);


    Camera2D camera = {
        .offset = {0, TOP_PADDING * PIXEL},
        .target = {0, 0},
        .rotation = 0.0f,
        .zoom = 1.0f
    };

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        drawMaze(gameTexture, 0);
        EndMode2D();
        EndDrawing();
    }

    UnloadImage(gameImage);
    UnloadTexture(gameTexture);
    CloseWindow();

    return 0;
}

