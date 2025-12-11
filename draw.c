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
    const float mx = ((float)GAME_WIDTH * SIZE) + NUDGE;
    const float my = ((float) game.level * GAME_HEIGHT * SIZE);
    const float mw = GAME_WIDTH * SIZE;
    const float mh = GAME_HEIGHT * SIZE;

    const Rectangle src = (Rectangle){mx, my, mw, mh};
    const Rectangle dst = (Rectangle){0, 0, mw * ZOOM, mh * ZOOM};

    DrawTexturePro(game_texture, src, dst, (Vector2){0, 0}, 0, WHITE);

    // --- draw dots and power-ups ---

    // source location from original artwork texture of a
    // dot and power up so that the dots are the same
    // color as you see in that image
    const float dot_x = 1, power_x = 1;
    float dot_y, power_y;

    switch (game.level) {
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
            printf("Invalid level: %d\n", game.level);
            exit(1);
    }

    Rectangle dotSrc = (Rectangle){dot_x * SIZE, dot_y * SIZE, SIZE, SIZE};
    Rectangle powerSrc = (Rectangle){power_x * SIZE, power_y * SIZE, SIZE, SIZE};

    for (int y = 0; y < GAME_HEIGHT; y+=1) {
        for (int x = 0; x < GAME_WIDTH; x+=1) {
            const tile_t tile = game.maze[y][x];

            const Rectangle rec = (Rectangle){(float)x*PIXEL, (float)y*PIXEL, PIXEL, PIXEL};

            if (tile == TILE_DOT) {
                DrawTexturePro(game_texture, dotSrc, rec, (Vector2){0, 0}, 0, WHITE);
            } else if (tile == TILE_POWER) {
                DrawTexturePro(game_texture, powerSrc, rec, (Vector2){0, 0}, 0, WHITE);
            }
        }
    }
}

void draw_player(void) {
    Vector2 sprite = player.sprite[player.dir];
    Rectangle src = (Rectangle){sprite.x + (float)player.frame * player.width, sprite.y, player.width, player.height};
    Rectangle dst = (Rectangle){player.pixel.x, player.pixel.y, player.width * ZOOM, player.height * ZOOM};
    DrawTexturePro(game_texture, src, dst, (Vector2){0, 0}, 0, WHITE);

}