//
// Created by Steve Spencer on 12/4/25.
//

#include <raylib.h>
#include "game.h"
#include "draw.h"

#include <stdio.h>
#include <stdlib.h>

void draw_maze(void) {
    // TODO need conversion between level and board

    // --- draw maze ---
    const float gap = TILE / 2.0f;
    const float mx = ((float)GAME_WIDTH * TILE) + gap;
    const float my = ((float) world.game.level * GAME_HEIGHT * TILE);
    const float mw = GAME_WIDTH * TILE;
    const float mh = GAME_HEIGHT * TILE;

    const Rectangle src = (Rectangle){mx, my, mw, mh};
    const Rectangle dst = (Rectangle){0, 0, mw, mh};

    DrawTexturePro(world.game_texture, src, dst, (Vector2){0, 0}, 0, WHITE);

    // --- draw dots and power-ups ---

    // source location from original artwork texture of a
    // dot and power up so that the dots are the same
    // color as you see in that image
    const float dot_x = 1, power_x = 1;
    float dot_y, power_y;

    switch (world.game.level) {
        case 0:
            dot_y = 1, power_y = 2;
            break;
        case 1:
            dot_y = 36, power_y = 35;
            break;
        case 2:
            dot_y = 63, power_y = 65;
            break;
        case 3:
            dot_y = 94, power_y = 96;
            break;
        case 4:
            dot_y = 125, power_y = 127;
            break;
        case 5:
            dot_y = 157, power_y = 158;
            break;
        default:
            printf("Invalid level: %d\n", world.game.level);
            exit(1);
    }

    const Rectangle dotSrc = (Rectangle){dot_x * TILE, dot_y * TILE, TILE, TILE};
    const Rectangle powerSrc = (Rectangle){power_x * TILE, power_y * TILE, TILE, TILE};

    for (int y = 0; y < GAME_HEIGHT; y+=1) {
        for (int x = 0; x < GAME_WIDTH; x+=1) {
            const tile_t tile = world.game.maze[y][x];

            const Rectangle rec = (Rectangle){(float)x*TILE, (float)y*TILE, TILE, TILE};

            if (tile == TILE_DOT) {
                DrawTexturePro(world.game_texture, dotSrc, rec, (Vector2){0, 0}, 0, WHITE);
            } else if (tile == TILE_POWER) {
                DrawTexturePro(world.game_texture, powerSrc, rec, (Vector2){0, 0}, 0, WHITE);
            }
        }
    }
}

void draw_pacman(void) {
    entity_t *p = &world.pacman;
    Vector2 sprite = (Vector2){p->sprite_x[p->dir], p->sprite_y[p->dir]};

    const Rectangle src = (Rectangle){sprite.x + (float)p->frame_index * SPRITE, sprite.y, SPRITE, SPRITE};
    const Rectangle dst = (Rectangle){p->pos.x, p->pos.y, SPRITE, SPRITE};
    DrawTexturePro(world.game_texture, src, dst, (Vector2){0, 0}, 0, WHITE);
}

void draw_ghosts(void) {
    for (int i = 0; i < GHOST_COUNT; i+=1) {
        entity_t *g = &world.ghosts[i];
        // if frightened
        // else if eaten
        // else normal
        const Rectangle src = (Rectangle){g->sprite_x[g->dir] + (float)g->frame_index * SPRITE, g->sprite_y[g->dir], SPRITE, SPRITE};
        const Rectangle dst = (Rectangle){g->pos.x, g->pos.y, SPRITE, SPRITE};
        DrawTexturePro(world.game_texture, src, dst, (Vector2){0, 0}, 0, WHITE);
    }
}

void draw_checkerboard(void) {
    const Color c1 = (Color){255, 255, 255, 130};
    const Color c2 = (Color){255, 255, 255, 90};
    const Color c3 = (Color){255, 0, 0, 150};
    int i = 0;
    for (int y = 0; y < GAME_HEIGHT; y+=1) {
        for (int x = 0; x < GAME_WIDTH; x+=1) {
            Color c = (i % 2 == 0) ? c1 : c2;
            if (y == 12 && (x == 13 || x == 14)) {
                c = c3;
            }
            DrawRectangleV((Vector2){x*TILE, y*TILE}, (Vector2){TILE, TILE}, c);
            i += 1;
        }
        i += 1;
    }
}