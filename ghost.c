//
// Created by Steve Spencer on 1/13/26.
//

#include "ghost.h"

#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "maze.h"
#include "level.h"
#include "prng.h"

static Vector2 pos_to_tile(const int x, const int y) {
    return (Vector2){
        floorf(((float) x /*- HALF*/) / TILE),
        floorf(((float) y/* - HALF*/) / TILE)
    };
}

static Vector2 pixel_to_tile(const Vector2 pixel) {
    return (Vector2){
        floorf(pixel.x / TILE),
        floorf(pixel.y / TILE)
    };
}

static Vector2 tile_to_pixel(const int x, const int y) {
    return (Vector2){(float)x * TILE, (float)y * TILE};
}

Vector2 get_next_tile(Vector2 tile, const Dir dir) {
    Vector2 next = (Vector2){tile.x, tile.y};
    switch (dir) {
        case UP: next.y--;
            return next;
        case DOWN: next.y++;
            return next;
        case LEFT: next.x--;
            return next;
        case RIGHT: next.x++;
            return next;
        default: return next;
    }
}


// ------------------- SCATTER -------------------
Vector2 blinky_scatter() {
    // pacman
    return tile_to_pixel(GAME_WIDTH-3, -3);
}

Vector2 inky_scatter() {
    // pacman
    return tile_to_pixel(GAME_WIDTH-1, GAME_HEIGHT);
}

Vector2 pinky_scatter() {
    // pacman
    return tile_to_pixel(2, -3);
}

Vector2 clyde_scatter() {
    //  pacman
    return tile_to_pixel(0, GAME_HEIGHT);
}


// ------------------- START -------------------
Vector2 blinky_start() {
    // pacman
    return tile_to_pixel(14, 14);
}

Vector2 inky_start() {
    // pacman
    return tile_to_pixel(12, 14);
}

Vector2 pinky_start() {
    // pacman
    return tile_to_pixel(14, 14);
}

Vector2 clyde_start() {
    //  pacman
    return tile_to_pixel(16, 14);
}

// ------------------- CHASE -------------------
Vector2 blinky_chase() {
    return (Vector2){(float) game.player.x, (float) game.player.y};
}

Vector2 inky_chase() {
    float vx = 0.0f, vy = 0.0f;

    // reproduces original chase bug for NORTH
    // to remove "bug", set vx=0
    switch (game.player.dir) {
        case UP: vx = -2.0f, vy = -2.0f;
            break;
        case DOWN: vy = 2.0f;
            break;
        case RIGHT: vx = 2.0f;
            break;
        default: vx = -2.0f;
            break;
    }

    const Vector2 player = pos_to_tile(game.player.x, game.player.y);
    const Vector2 pivot = (Vector2){player.x + vx, player.y + vy};
    const Vector2 blinky = pos_to_tile(game.ghosts[BLINKY].x, game.ghosts[BLINKY].y); // YES, BLINKY
    const float dx = pivot.x - blinky.x;
    const float dy = pivot.y - blinky.y;
    return tile_to_pixel(blinky.x + 2 * dx, blinky.y + 2 * dy);
}

Vector2 pinky_chase() {
    float vx = 0.0f, vy = 0.0f;

    // reproduces original chase bug for NORTH
    // to remove "bug", set vx=0
    switch (game.player.dir) {
        case UP: vx = -4.0f, vy = -4.0f;
            break;
        case DOWN: vy = 4.0f;
            break;
        case RIGHT: vx = 4.0f;
            break;
        default: vx = -4.0f;
            break;
    }

    const Vector2 player = pos_to_tile(game.player.x, game.player.y);
    return tile_to_pixel(player.x + vx, player.y + vy);
}

Vector2 clyde_chase() {
    Actor *pacman = &game.player;
    Actor *clyde = &game.ghosts[CLYDE];
    const Vector2 pv = pos_to_tile(pacman->x, pacman->y);
    const Vector2 sv = pos_to_tile(clyde->x, clyde->y);

    const float dx = pv.x - sv.x;
    const float dy = pv.y - sv.y;
    const float dist = sqrtf(dx * dx + dy * dy);
    if (dist >= 8.0f) {
        return tile_to_pixel(pv.x, pv.y);
    }

    return clyde_scatter();
}

// ------------------- LEAVE -------------------

bool blinky_leave() {
    return true;
}

bool inky_leave() {
    // TODO fix INKY leave timing
    return game.dots_eaten > 30 || game.level_frame_count > SECONDS_TO_FRAMES(7);
}

bool pinky_leave() {
    return game.level_frame_count > SECONDS_TO_FRAMES(1);
}

bool clyde_leave() {
    // TODO fix CLYDE leave timing
    return game.dots_eaten > 60 || game.level_frame_count > SECONDS_TO_FRAMES(15);
}

void init_base_ghost(Actor *g, int id, int x, int y, [[maybe_unused]] GhostState state, Dir dir) {

    g->id = id;
    g->x = x * TILE - HALF;
    g->y = y * TILE;
    g->target = (Vector2){};
    g->state = state;
    g->dir = dir;
    g->is_player = false;
    g->frame_count = 0;
    g->frame_index = 0;
    g->pixels_moved = 0;
    // Phase controls initial bounce direction: UP starts upward, DOWN starts downward.

    g->speed_idx = 0;
    // g->is_frightened = false;
    g->reverse = false;
}

void init_blinky(Actor *g) {
    init_base_ghost(g, BLINKY, 14, 11, SCATTER, LEFT);
    g->name = "blinky";
    g->chase = blinky_chase;
    g->leave = blinky_leave;
    g->scatter = blinky_scatter;
}

void init_inky(Actor *g) {
    init_base_ghost(g, INKY, 12, 14, IN_HOUSE, UP);
    g->name = "inky";
    //init_base_ghost(g, 12, 11, SCATTER, LEFT);
    g->chase = inky_chase;
    g->leave = inky_leave;
    g->scatter = inky_scatter;
}

void init_pinky(Actor *g) {
    init_base_ghost(g, PINKY, 14, 14, IN_HOUSE, DOWN);
    g->name = "pinky";
    //init_base_ghost(g, 15, 11, SCATTER, LEFT);
    g->chase = pinky_chase;
    g->leave = pinky_leave;
    g->scatter = pinky_scatter;
}

void init_clyde(Actor *g) {
    init_base_ghost(g, CLYDE, 16, 14, IN_HOUSE, UP);
    g->name = "clyde";
    //init_base_ghost(g, 16, 11, SCATTER, RIGHT);
    g->chase = clyde_chase;
    g->leave = clyde_leave;
    g->scatter = clyde_scatter;
}

static float distance_between(Vector2 a, Vector2 b) {
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

static Vector2 normalize_tunnel_tile(Vector2 tile) {
    if (tile.x < 0) tile.x += GAME_WIDTH;
    if (tile.x >= GAME_WIDTH) tile.x -= GAME_WIDTH;
    return tile;
}



static bool is_valid_ghost_tile(const Vector2 tile, const GhostState state) {
    const Vector2 wrapped_tile = normalize_tunnel_tile(tile);
    if (wrapped_tile.x < 0 || wrapped_tile.x >= GAME_WIDTH || wrapped_tile.y < 0 || wrapped_tile.y >= GAME_HEIGHT) {
        return false;
    }

    int x = (int) wrapped_tile.x;
    int y = (int) wrapped_tile.y;
    const Tile t = game.maze[y][x];

    if (state == CHASE || state == SCATTER) {
        if (t == TILE_WALL || is_ghost_wall(x, y)) return false;
    }

    if (t == TILE_WALL) return false;
    if (t == TILE_DOOR) return (state == LEAVING_HOUSE || state == EATEN);

    return true;
}

static Dir choose_ghost_direction(Actor *g) {
    if (g->reverse) {
        g->reverse = false;
        return opposite_dir[g->dir];
    }


    int valid_dirs = 0;
    const Vector2 tile = pos_to_tile(g->x, g->y);

    if (g->state == FRIGHTENED) {
        if (is_valid_ghost_tile(get_next_tile(tile, UP), FRIGHTENED)) valid_dirs |= UP_MASK;
        if (is_valid_ghost_tile(get_next_tile(tile, RIGHT), FRIGHTENED)) valid_dirs |= RIGHT_MASK;
        if (is_valid_ghost_tile(get_next_tile(tile, DOWN), FRIGHTENED)) valid_dirs |= DOWN_MASK;
        if (is_valid_ghost_tile(get_next_tile(tile, LEFT), FRIGHTENED)) valid_dirs |= LEFT_MASK;

        return choose_frightened_direction(g->dir, valid_dirs);
    }

    static constexpr Dir dir_priority[4] = {UP, LEFT, DOWN, RIGHT};
    for (int i = 0; i < 4; i++) {
        const Dir dir = dir_priority[i];
        if (dir == opposite_dir[g->dir]) continue;
        const Vector2 next_tile = get_next_tile(tile, dir);
        if (is_valid_ghost_tile(next_tile, g->state)) {
            valid_dirs |= 1 << dir;
        }
    }

    if (valid_dirs == 0) return g->dir;

    Dir best_dir = DIR_NONE;
    float min_distance = MAXFLOAT;
    const Vector2 target_tile = pixel_to_tile(g->target);
    for (int i = 0; i < 4; i++) {
        const Dir dir = dir_priority[i];
        if (valid_dirs & 1 << dir) {
            const Vector2 next_tile = normalize_tunnel_tile(get_next_tile(tile, dir));
            const float dist = distance_between(target_tile, next_tile);
            if (dist < min_distance) {
                min_distance = dist;
                best_dir = dir;
            }
        }
    }

    return best_dir;
}

static void update_ghost_frame(Actor *g) {
    constexpr int frames_per_state = 15;
    constexpr int cycle_length = 2 * frames_per_state; // 30 frames for full cycle
    const int frame_in_cycle = g->frame_count % cycle_length;

    if (frame_in_cycle < frames_per_state) {
        g->frame_index = 0; // First pose
    } else {
        g->frame_index = 1;
    }

    g->frame_count++;
    if (g->frame_count > cycle_length) g->frame_count = 0;
}

static void update_ghost(Actor *g) {
    static constexpr Vector2 eaten_target = (Vector2){(float)14 * TILE, (float)11 * TILE};

    update_ghost_frame(g);

    if (g->state == IN_HOUSE) {
        if (g->leave()) {
            g->state = LEAVING_HOUSE;
        } else {
            int y = g->y;
            update_actor(g, game.level_spec.ghost_tunnel_speed);//get_ghost_speed(g));
            bool moved = y != g->y;
            if (moved && (g->y == (14 * TILE + HALF) || g->y == (14 * TILE - HALF))) {
                g->dir = opposite_dir[g->dir];
            }
        }

        return;
    }

    if (g->state == LEAVING_HOUSE) {
        uint32_t speed = game.level_spec.ghost_tunnel_speed;
        if (g->x < HOUSE_X) {
            g->dir = RIGHT;
            update_actor(g, speed);
        } else if (g->x > HOUSE_X) {
            g->dir = LEFT;
            update_actor(g, speed);
        } else {
            if (g->y > HOUSE_Y) {
                g->dir = UP;
                update_actor(g, speed);
            } else {
                g->state = game.ghost_state;
                g->target = g->state == SCATTER ? g->scatter() : g->chase();
                g->dir = choose_ghost_direction(g);
            }
        }

        return;
    }

    if (g->state == EATEN) {
        // EATEN routing targets tile (14,11). Transition on the same pixel target
        // so ghosts don't keep re-pathing and flipping eyes at the house entrance.
        if (g->x == (int)eaten_target.x && g->y == (int)eaten_target.y) {
            printf("EYES MADE IT TO HOUSE DOOR");
            g->state = game.ghost_state;
            return;
        }
    }
    if (is_centered(g->x, g->y)) {
        Vector2 target;
        if (g->state == EATEN) {
            target = eaten_target;

        } else if (g->state == SCATTER) {
            target = g->scatter();
        } else if (g->state == FRIGHTENED) {
            // target ignored in FRIGHTENED mode
            target = (Vector2){};
        } else {
            target = g->chase();
        }
        g->target = target;
        const Dir dir = choose_ghost_direction(g);
        if (dir != g->dir && g->pixels_moved > TILE) {
            // printf("changing direction from %s to %s\n", dir_to_string(g->dir), dir_to_string(dir));
            g->dir = dir;
            g->pixels_moved = 0;
        }
    }

    update_actor(g, get_ghost_speed(g));
    if (g->x < 0) g->x += GAME_WIDTH * TILE;
    if (g->x >= GAME_WIDTH * TILE) g->x -= GAME_WIDTH * TILE;
    g->pixels_moved++;
}

void update_ghosts() {
    for (int i = 0; i < NUM_GHOSTS; i++) {
        update_ghost(&game.ghosts[i]);
    }
}
