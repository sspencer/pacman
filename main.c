#include <stdio.h>

#include "raylib.h"

#define SCREEN_HEIGHT 36
#define SCREEN_WIDTH  28
#define GAME_WIDTH 28
#define GAME_HEIGHT 31
#define TOP_PADDING 3
#define ZOOM 3
#define SIZE 8
#define PIXEL (SIZE * ZOOM)

static void drawMaze(Texture2D texture) {
    float board = 0;
    float x = (GAME_WIDTH * SIZE) + SIZE / 2.0;
    float y = (board * GAME_HEIGHT * SIZE);
    float w = GAME_WIDTH * SIZE;
    float h = GAME_HEIGHT * SIZE;

    Rectangle src = (Rectangle){x, y, w, h};
    Rectangle dst = (Rectangle){0, 0, w*ZOOM, h*ZOOM};

    DrawTexturePro(texture, src, dst, (Vector2){0, 0}, 0, WHITE);
}

int main(void) {
    InitWindow(SCREEN_WIDTH * PIXEL, SCREEN_HEIGHT * PIXEL, "Ms. Pacman");
    SetWindowPosition(GetMonitorWidth(0) - (SCREEN_WIDTH * PIXEL), 0);
    SetTraceLogLevel(LOG_WARNING);
    SetTargetFPS(60);
    Texture2D gameTexture = LoadTexture("assets/game.png");
    printf("%d x %d\n", gameTexture.width, gameTexture.height);

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
        drawMaze(gameTexture);
        EndMode2D();
        EndDrawing();
    }

    UnloadTexture(gameTexture);
    CloseWindow();

    return 0;
}

