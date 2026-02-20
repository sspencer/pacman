//
// Created by Steve Spencer on 1/8/26.
//

#include "player.h"

#include "level.h"

void init_pacman(Actor *pacman) {
    pacman->x = 108;
    pacman->y = 184;
    pacman->dir = LEFT;
    pacman->req_dir = LEFT;
    pacman->is_player = true;
    //pacman->is_frightened = false;
    pacman->speed_idx = 0;
    pacman->frame_count = 0;
    pacman->frame_index = 0;
    pacman->frames_to_pause = 0;
}

static bool can_pacman_move(const int tx, const int ty, Dir dir) {
    int nx = tx, ny = ty;
    switch (dir) {
        case UP: ny--;
            break;
        case DOWN: ny++;
            break;
        case LEFT: nx--;
            break;
        case RIGHT: nx++;
            break;
        default: return false;
    }
    // Tunnel wrap-around (Pacman-specific left/right tunnels)
    if (nx < 0) nx = GAME_WIDTH - 1;
    if (nx >= GAME_WIDTH) nx = 0;
    if (ny < 0 || ny >= GAME_HEIGHT || nx < 0 || nx >= GAME_WIDTH) return false;

    return (game.maze[ny][nx] != TILE_WALL && game.maze[ny][nx] != TILE_DOOR);
}

static void update_pacman_frame(Actor *pacman) {
    constexpr int frames_per_state = 5;
    constexpr int cycle_length = 4 * frames_per_state;
    const int frame_in_cycle = pacman->frame_count % cycle_length;

    if (frame_in_cycle < frames_per_state) {
        pacman->frame_index = 2; // closed
    } else if (frame_in_cycle < 2 * frames_per_state) {
        pacman->frame_index = 1; // half open
    } else if (frame_in_cycle < 3 * frames_per_state) {
        pacman->frame_index = 0; // fully open
    } else {
        pacman->frame_index = 1; // half open (returning
    }

    pacman->frame_count++;
    if (pacman->frame_count > cycle_length) {
        pacman->frame_count = 0;
    }
}

void update_pacman(void) {
    Actor *p = &game.player;
    update_pacman_frame(p); // animate pacman mouth

    if (IsKeyDown(KEY_UP)) p->req_dir = UP;
    else if (IsKeyDown(KEY_DOWN)) p->req_dir = DOWN;
    else if (IsKeyDown(KEY_LEFT)) p->req_dir = LEFT;
    else if (IsKeyDown(KEY_RIGHT)) p->req_dir = RIGHT;

    // Cornering threshold (in pixels)
    constexpr int threshold = 3;

    const int tx = p->x / TILE;
    const int ty = p->y / TILE;
    const int ox = p->x % TILE;
    const int oy = p->y % TILE;

    // Handle direction change
    if (p->req_dir != p->dir && p->req_dir != DIR_NONE) {
        if ((p->dir % 2) == (p->req_dir % 2)) {
            // Opposite direction: always allow
            p->dir = p->req_dir;
        } else {
            // Perpendicular turn: check if near intersection
            const bool near_x = (ox <= threshold || ox >= TILE - threshold);
            const bool near_y = (oy <= threshold || oy >= TILE - threshold);

            if (p->dir == UP || p->dir == DOWN) {
                if (near_y) {
                    const int target_ty = (oy <= HALF) ? ty : ty + 1;
                    if (can_pacman_move(tx, target_ty, p->req_dir)) {
                        p->dir = p->req_dir;
                    }
                }
            } else {
                if (near_x) {
                    const int target_tx = (ox <= HALF) ? tx : tx + 1;
                    if (can_pacman_move(target_tx, ty, p->req_dir)) {
                        p->dir = p->req_dir;
                    }
                }
            }
        }
    }

    // Move actor if not blocked
    bool can_move = false;
    if (p->dir == UP || p->dir == DOWN) {
        if (oy != 0) {
            can_move = true;
        } else {
            const int target_tx = (ox <= HALF) ? tx : tx + 1;
            can_move = can_pacman_move(target_tx, ty, p->dir);
        }
    } else if (p->dir == LEFT || p->dir == RIGHT) {
        if (ox != 0) {
            can_move = true;
        } else {
            const int target_ty = (oy <= HALF) ? ty : ty + 1;
            can_move = can_pacman_move(tx, target_ty, p->dir);
        }
    }

    if (can_move) {
        update_actor(p, get_player_speed());
    }

    // Eat dots/powerups using center-aligned tile coordinates
    const int ctx = (p->x + HALF) / TILE;
    const int cty = (p->y + HALF) / TILE;
    if (ctx >= 0 && ctx < GAME_WIDTH && cty >= 0 && cty < GAME_HEIGHT) {
        uint8_t (*maze)[GAME_WIDTH] = game.maze;
        if (maze[cty][ctx] == TILE_DOT) {
            maze[cty][ctx] = TILE_EMPTY;
            event_stack_push((Event){PLAYER_ATE_DOT, 0, ctx, cty});
        } else if (maze[cty][ctx] == TILE_POWER) {
            maze[cty][ctx] = TILE_EMPTY;
            event_stack_push((Event){PLAYER_ATE_POWERUP, 0, ctx, cty});
        }
    }

    // tunnel warp
    if (p->x < 0) p->x += GAME_WIDTH * TILE;
    if (p->x >= GAME_WIDTH * TILE) p->x -= GAME_WIDTH * TILE;
}
