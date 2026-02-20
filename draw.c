//
// Created by Steve Spencer on 1/8/26.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <raylib.h>
#include "game.h"
#include "draw.h"
#include "maze.h"

void draw_maze(void) {
    const int maze_num = get_maze_num(game.level);

    // --- draw maze ---
    constexpr float mw = GAME_WIDTH * TILE;
    constexpr float mh = GAME_HEIGHT * TILE;

    const Rectangle src = (Rectangle){
        (float) GAME_WIDTH * TILE + HALF, (float) maze_num * GAME_HEIGHT * TILE, mw, mh
    };
    const Rectangle dst = (Rectangle){0, 0, mw, mh};

    DrawTexturePro(game.sprite_texture, src, dst, (Vector2){0, 0}, 0, WHITE);

    // --- draw dots and power-ups ---

    // source location from original artwork texture of a
    // dot and power up so that the dots are the same
    // color as you see in that image
    static constexpr float dots[5] = {1, 36, 63, 94, 125}; // 125, 157
    static constexpr float powers[5] = {2, 35, 65, 96, 127}; // 127, 158

    const Vector2 dot = (Vector2){1, dots[maze_num]};
    const Vector2 power = (Vector2){1, powers[maze_num]};

    const Rectangle dotSrc = (Rectangle){dot.x * TILE, dot.y * TILE, TILE, TILE};
    const Rectangle powerSrc = (Rectangle){power.x * TILE, power.y * TILE, TILE, TILE};

    for (int y = 0; y < GAME_HEIGHT; y += 1) {
        for (int x = 0; x < GAME_WIDTH; x += 1) {
            const Tile tile = game.maze[y][x];

            const Rectangle rec = (Rectangle){(float) x * TILE, (float) y * TILE, TILE, TILE};

            if (tile == TILE_DOT) {
                DrawTexturePro(game.sprite_texture, dotSrc, rec, (Vector2){0, 0}, 0, WHITE);
            } else if (tile == TILE_POWER) {
                DrawTexturePro(game.sprite_texture, powerSrc, rec, (Vector2){0, 0}, 0, WHITE);
            }
        }
    }
}

static void draw_sprite(const float x, const float y, const float sx, const float sy, const float frame_index) {
    const Rectangle src = (Rectangle){sx + frame_index * SPRITE, sy, SPRITE, SPRITE};
    Rectangle dst = (Rectangle){x, y, SPRITE, SPRITE};
    DrawTexturePro(game.sprite_texture, src, dst, (Vector2){0, 0}, 0, WHITE);
    // Rectangle center = (Rectangle){x + TILE-HALF/2, y + TILE-HALF/2, HALF, HALF};
    // DrawRectangleRec(center, GREEN);
    // int tile_x = (int) floorf(x / TILE);
    // int tile_y = (int) floorf(y / TILE);
    // if (tile_x < 0) tile_x = 0;
    // if (tile_y < 0) tile_y = 0;
    // if (tile_x >= GAME_WIDTH) tile_x = GAME_WIDTH-1;
    // if (tile_y >= GAME_HEIGHT) tile_y = GAME_HEIGHT-1;
    // if (game.maze[tile_y][tile_x] == TILE_TUNNEL) {
    //     Rectangle mazer = (Rectangle){x, y, SPRITE, SPRITE};
    //     DrawRectangleRec(mazer, BLUE);
    //
    // }
}

static Vector2 get_eye_sprite(Dir dir) {
    static constexpr float eyes_x[4] = { 616, 584, 632, 600 };

    return (Vector2){eyes_x[dir], 80};
}

static Vector2 get_frightened_sprite(Dir dir) {
    // TODO change delta_time to use frame count
    static constexpr float blues_x[4] = {584, 584, 584, 584}; // north, south, east, west
    static constexpr float whites_x[4] = {616, 616, 616, 616}; // north, south, east, west

    if (game.frightened_frames >= game.frightened_flashes * FRIGHTENED_FLASH_FRAMES) {
        return (Vector2){blues_x[dir], 64};
    }

    const int n = (game.frightened_frames / FRIGHTENED_FLASH_FRAMES) + 1;
    if (n % 2 == 0) {
        return (Vector2){blues_x[dir], 64};
    }

    return (Vector2){whites_x[dir], 64};
}

static Vector2 get_ghost_sprite(int id, Dir dir) {
    static constexpr float ghosts_y[4] = {64, 96, 80, 112}; // blinky, inky, pinky, sue
    static constexpr float ghosts_x[4] = { 520, 456, 552, 488}; // UP, RIGHT, DOWN, LEFT

    return (Vector2){ghosts_x[dir], ghosts_y[id]};
}

/*
void draw_food() {
    const Actor *a = &game.food;
    DrawRectangleV((Vector2){(float) a->x, (float) a->y},
                   (Vector2){SPRITE, SPRITE}, (Color){255, 255, 0, 82});
    //DrawCircleV((Vector2){(float) a->x + TILE, (float) a->y + TILE}, TILE,
    //            (Color){255, 0, 0, 51}); // drawn from center
    DrawCircleV((Vector2){(float) a->x + TILE, (float) a->y + TILE}, 2,
                (Color){255, 0, 0, 82}); // drawn from center
}

void draw_food_old() {
    const Actor *f = &game.food;
    static float sprites[12] = {0, 0, 0, 0, 0, 504, 520, 536, 552, 568, 584, 600};
    // cherry, strawberry, orange, pretzel, apple, pear, banana
    Vector2 sprite = (Vector2){sprites[f->name], 0};

    Vector2 center = (Vector2){f->x, f->y};
    draw_sprite(center, sprite, 0);
}
*/
void draw_ghost_score(int x, int y, int score) {
    static constexpr float score_x[4] = { 456, 472, 488, 504 };
    static constexpr float score_y = 128;

    draw_sprite((float)x, (float)y, score_x[score % 4], score_y, 0);
}

void draw_ghosts(void) {
    for (int i = 0; i < NUM_GHOSTS; i++) {
        Actor *g = &game.ghosts[i];
        Vector2 sprite;
        int frame = g->frame_index;

        if (g->state == EATEN) {
            sprite = get_eye_sprite(g->dir);
            // Eyes are directional but not animated; advancing frame index
            // shifts to neighboring atlas columns and looks like dir flicker.
            frame = 0;
        } else if (g->state == FRIGHTENED) {
            // TODO change delta_time to frame count
            sprite = get_frightened_sprite(g->dir);
        } else {
            sprite = get_ghost_sprite(i, g->dir);
        }

        draw_sprite((float)g->x, (float)g->y, sprite.x, sprite.y, (float)frame);
    }
}

void draw_pacman(Actor *p) {
    static constexpr float sprites[4] = {1024, 992, 1040, 1008}; // (y): up, right, down, left
    draw_sprite((float) p->x, (float) p->y, 456, sprites[p->dir], p->frame_index);
}

void draw_ms_pacman(Actor *p) {
    static constexpr float sprites[4] = {32, 0, 48, 16}; // (y): up, right, down, left
    draw_sprite((float) p->x, (float) p->y, 456, sprites[p->dir], p->frame_index);
}

void draw_checkerboard(void) {
    constexpr Color c1 = (Color){255, 255, 255, 120};
    constexpr Color c2 = (Color){255, 255, 255, 80};
    int i = 0;
    for (int y = 0; y < GAME_HEIGHT; y += 1) {
        for (int x = 0; x < GAME_WIDTH; x += 1) {
            const Rectangle rec = (Rectangle){(float) x * TILE, (float) y * TILE, TILE, TILE};
            const Tile tile = game.maze[y][x];

            Color c = (i % 2 == 0) ? c1 : c2;
            DrawRectangleRec(rec, c);
            i += 1;

            if (is_ghost_wall(x, y)) {
                DrawRectangleRec(rec, RED);
            } else if (tile == TILE_TUNNEL) {
                DrawRectangleRec(rec, PINK);
            }
        }
        i += 1;
    }

}

void draw_text(const char *text, int x, int y, Color color) {
    float fx = 0, fy = 0;

    for (int i = 0; text[i] != '\0'; i++) {
        unsigned char c = text[i];

        if (c >= '0' && c <= '9') {
            fx = (float) (c - '0');
            fy = 2;
        } else if (c >= 'A' && c <= 'O') {
            fx = (float) (c - 'A');
            fy = 0;
        } else if (c >= 'P' && c <= 'Z') {
            fx = (float) (c - 'P');
            fy = 1;
        } else if (c == '!') {
            fx = 11;
            fy = 1;
        } else if (c == '/') {
            fx = 10;
            fy = 2;
        } else if (c == '-') {
            fx = 11;
            fy = 2;
        } else if (c == '"') {
            fx = 12;
            fy = 2;
        } else {
            // Invalid or space: skip drawing but still advance position
            continue;
        }

        const Rectangle src = (Rectangle){fx * TILE, fy * TILE, TILE, TILE};
        const Rectangle dst = (Rectangle){(float) (x + i) * TILE, (float) y * TILE, TILE, TILE};
        DrawTexturePro(game.font_texture, src, dst, (Vector2){0.0f, 0.0f}, 0.0f, color);
    }
}

void draw_score_labels(void) {
    draw_text("1UP", 3, 0, WHITE);
    draw_text("HIGH SCORE", 9, 0, WHITE);
    draw_text("2UP", 22, 0, WHITE);
}

void draw_scores(void) {
    static char scoreBuffer[32];
    snprintf(scoreBuffer, sizeof(scoreBuffer), "%d", game.score);
    draw_text(scoreBuffer, 3, 1, WHITE);
    draw_text("0", 13, 1, WHITE);
    draw_text("0", 24, 1, WHITE);
}

void draw_level(void) {
    static char buf[32];
    snprintf(buf, sizeof(buf), "LEVEL %d", game.level);
    draw_text(buf, 1, GAME_HEIGHT, WHITE);
}
