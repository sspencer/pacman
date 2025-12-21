//
// Created by Steve Spencer on 12/4/25.
//

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "draw.h"
#include "ghost.h"

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

    DrawTexturePro(world.texture, src, dst, (Vector2){0, 0}, 0, WHITE);

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
                DrawTexturePro(world.texture, dotSrc, rec, (Vector2){0, 0}, 0, WHITE);
            } else if (tile == TILE_POWER) {
                DrawTexturePro(world.texture, powerSrc, rec, (Vector2){0, 0}, 0, WHITE);
            } else if (tile == TILE_TUNNEL) {
                DrawRectangleV((Vector2){x*TILE, y*TILE}, (Vector2){TILE, TILE}, SKYBLUE);
            }
        }
    }
}

void draw_pacman(void) {
    entity_t *p = &world.pacman;
    Vector2 sprite = (Vector2){p->sprite[p->dir].x, p->sprite[p->dir].y};

    const Rectangle src = (Rectangle){sprite.x + (float)p->frame_index * SPRITE, sprite.y, SPRITE, SPRITE};
    const Rectangle dst = (Rectangle){p->pos.x, p->pos.y, SPRITE, SPRITE};
    DrawTexturePro(world.texture, src, dst, (Vector2){0, 0}, 0, WHITE);

    // Debug overlay to visualize alignment between intended tile center and actual position
    if (world.game.debug) {
        const Rectangle intended = (Rectangle){p->tile.x * TILE, p->tile.y * TILE, SPRITE, SPRITE};
        const Rectangle actual   = (Rectangle){p->pos.x, p->pos.y, SPRITE, SPRITE};
        DrawRectangleLinesEx(intended, 1, GREEN);
        DrawRectangleLinesEx(actual, 1, RED);

        const float dx = p->pos.x - p->tile.x * TILE;
        const float dy = p->pos.y - p->tile.y * TILE;
        DrawText(TextFormat("dx=%.1f dy=%.1f", dx, dy), (int)p->pos.x, (int)(p->pos.y - 8), 8, YELLOW);
    }
}

void draw_ghosts() {
    const static Vector2 eyes[DIR_COUNT] = {
        {616, 80},
        {632, 80},
        {584, 80},
        {600, 80}
    }, fright_blue[DIR_COUNT] = {
        {584, 64},
        {584, 64},
        {584, 64},
        {584, 64}
    }, fright_white[DIR_COUNT] = {
        {616, 64},
        {616, 64},
        {616, 64},
        {616, 64}
    };

    for (int i = 0; i < GHOST_COUNT; i+=1) {
        entity_t *g = &world.ghosts[i];

        Vector2 sprite;
        float frame_index = g->frame_index;

        if (g->sprite_type == SPRITE_EYES) {
            sprite = eyes[g->dir];
            frame_index = 0;
        } else if (g->sprite_type == SPRITE_BLUE) {
            sprite = fright_blue[g->dir];
        } else if (g->sprite_type == SPRITE_WHITE) {
            sprite = fright_white[g->dir];
        }  else {
            sprite = g->sprite[g->dir];
        }

        const Rectangle src = (Rectangle){sprite.x + frame_index * SPRITE, sprite.y, SPRITE, SPRITE};
         Rectangle dst = (Rectangle){g->pos.x, g->pos.y, SPRITE, SPRITE};
        if (is_in_house(g)) {
            dst.x -= (TILE/2.0f);
        }

        DrawTexturePro(world.texture, src, dst, (Vector2){0, 0}, 0, WHITE);
        /*
        if (world.game.debug) {
            Color c;
            if (i == GHOST_BLINKY) c = RED;
            else if (i == GHOST_PINKY) c = MAGENTA;
            else if (i == GHOST_INKY) c = SKYBLUE;
            else c = ORANGE;

            DrawRectangleLinesEx((Rectangle){g->target.x *TILE, g->target.y*TILE, SPRITE, SPRITE}, 2, c);
        }
        */
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

void draw_text(const char *text, int x, int y, Color color) {
    float fx = 0, fy = 0;
    bool valid = false;

    for (int i = 0; text[i] != '\0'; i++) {
        unsigned char c = text[i];

        if (c >= '0' && c <= '9') {
            // 0 - 9
            fx = (float)(c - '0');
            fy = 2;
            valid = true;
        } else if (c >= 'A' && c <= 'O') {
            // A - O
            fx = (float)(c - 'A');
            fy = 0;
            valid = true;
        } else if (c >= 'P' && c <= 'Z') {
            // P - Z
            fx = (float)(c - 'P');
            fy = 1;
            valid = true;
        } else if (c == '!') {
            fx = 11;
            fy = 1;
            valid = true;
        } else if (c == '/') {
            fx = 10;
            fy = 2;
            valid = true;
        } else if (c == '-') {
            fx = 11;
            fy = 2;
            valid = true;
        } else if (c == '"') {
            fx = 12;
            fy = 2;
            valid = true;
        } else {
            // Invalid or space: skip drawing but still advance position
            continue;
        }


        Rectangle src = { fx * TILE, fy * TILE, TILE, TILE };
        Rectangle dst = { (float)(x + i) * TILE, (float)y * TILE, TILE, TILE };
        DrawTexturePro(world.font, src, dst, (Vector2){0.0f, 0.0f}, 0.0f, color);
    }
}