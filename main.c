#include <stdio.h>

#include "raylib.h"
#include "game.h"
#include "maze.h"
#include "draw.h"
#include "entity.h"




static void update_world(world_t world) {
    if (world.game->paused) return;

    if (IsKeyPressed(KEY_P)) {
        world.game->paused = !world.game->paused;
        return;
    }

    if (IsKeyPressed(KEY_N)) {
        world.game->level = (world.game->level + 1) % 5;
        map_maze(world.game, world.image);
    }

    if (IsKeyPressed(KEY_LEFT)) {
        world.player->dir = DIR_WEST;
        world.player->next_vel = (Vector2){-1, 0};
    }

    if (IsKeyPressed(KEY_RIGHT)) {
        world.player->dir = DIR_EAST;
        world.player->next_vel = (Vector2){1, 0};
    }

    if (IsKeyPressed(KEY_UP)) {
        world.player->dir = DIR_NORTH;
        world.player->next_vel = (Vector2){0, -1};
    }

    if (IsKeyPressed(KEY_DOWN)) {
        world.player->dir = DIR_SOUTH;
        world.player->next_vel = (Vector2){0, 1};
    }

    update_player(world.game, world.player);
}

static void draw_world(world_t world) {
    draw_maze(world.texture, world.game);
    draw_player(world.texture, world.player);
}

int main(void) {
    InitWindow(SCREEN_WIDTH * PIXEL, SCREEN_HEIGHT * PIXEL, "Ms. Pacman");
    SetWindowPosition(GetMonitorWidth(0) - (SCREEN_WIDTH * PIXEL), 0);
    SetTraceLogLevel(LOG_WARNING);
    SetTargetFPS(60);

    Texture2D game_texture = LoadTexture("assets/game.png");
    Image game_image = LoadImageFromTexture(game_texture);

    game_t game = {.level=0, .paused=false};
    map_maze(&game, &game_image);

    entity_t player = {};
    init_player(&player);

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

    world_t world = {.game=&game, .image=&game_image, .player=&player, .texture=game_texture};

    while (!WindowShouldClose()) {
        update_world(world);

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        draw_world(world);
        //draw_maze(&game, game_texture);
        EndMode2D();
        EndDrawing();
    }

    UnloadImage(game_image);
    UnloadTexture(game_texture);
    CloseWindow();

    return 0;
}