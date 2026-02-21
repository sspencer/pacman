//
// Created by Steve Spencer on 1/8/26.
//
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "draw.h"
#include "maze.h"
#include "player.h"
#include "ghost.h"
#include "level.h"
#include "prng.h"

void init_game(void) {
    game.is_pacman = true; // false is ms_pacman
    game.score = 0;
    game.draw_xrays = false;
    game.font_texture = LoadTexture("assets/text.png");
    game.sprite_texture = LoadTexture("assets/game.png");
    game.draw_targets = false;
}

void init_level(int level) {
    game.level = level;
    game.level_spec = get_level_spec(level);
    game.is_paused = false;
    game.dots_eaten = 0;
    game.frame_count = 0;
    game.level_frame_count = 0;
    game.frightened_frames = 0;
    game.frightened_flashes = 0;
    game.ghost_state = SCATTER;
    game.ghost_phase_index = 0;
    game.ghost_phase_change = get_ghost_mode_phase(level, 0).duration_frames;
    reset_prng();
}

const char *dir_to_string(const Dir dir) {
    switch (dir) {
        case UP: return "UP";
        case RIGHT: return "RIGHT";
        case DOWN: return "DOWN";
        case LEFT: return "LEFT";
        case DIR_NONE: return "NONE";
        default: return "DIRECTION UNKNOWN";
    }
}

bool is_centered(const int x, const int y) {
    return (x % TILE) == 0 && (y % TILE) == 0;
}

LevelSpecTable get_level_spec(const int level) {
    constexpr size_t table_count = LEVEL_SPEC_TABLE_SIZE;
    for (size_t i = 0; i < table_count; i++) {
        if (level >= pacman_level_table->level_min && level <= pacman_level_table->level_max) {
            return pacman_level_table[i];
        }
    }

    return pacman_level_table[0];
}

static int get_tile_coord(const int pos, const int limit) {
    int tile = pos / TILE;
    if (tile < 0) tile = 0;
    if (tile >= limit) tile = limit - 1;
    return tile;
}

static int get_tile_x(const Actor *ghost) {
    return get_tile_coord(ghost->x, GAME_WIDTH);
}

static int get_tile_y(const Actor *ghost) {
    return get_tile_coord(ghost->y, GAME_HEIGHT);
}

// TODO: determine if ghost is in Elroy
uint32_t get_ghost_speed(const Actor *g) {
    if (game.maze[get_tile_y(g)][get_tile_x(g)] == TILE_TUNNEL) {
        return game.level_spec.ghost_tunnel_speed;
    }

    if (g->state == EATEN) {
        return SPEED_80;
    }

    if (g->state == FRIGHTENED) {
        return game.level_spec.ghost_fright_speed;
    }

    return game.level_spec.ghost_speed;
}

int get_ghost_score(int ghost_eaten) {
    int ghost_scores[4] = {200, 400, 800, 1600};
    return ghost_scores[ghost_eaten % 4];
}

uint32_t get_player_speed(void) {
    if (game.is_fright_mode) {
        return game.level_spec.pacman_fright_speed;
    }

    return game.level_spec.pacman_speed;
}

GhostModePhase get_ghost_mode_phase(const int level, const size_t index) {
    if (index >= GHOST_MODE_PHASE_COUNT) {
        return (GhostModePhase){.state = CHASE, .duration_frames = -1};
    }

    for (size_t i = 0; i < GHOST_MODE_PHASE_TABLE_SIZE; i++) {
        const GhostModePhaseTable *table = &ghost_mode_phase_table[i];
        if (level >= table->level_min && level <= table->level_max) {
            return table->phases[index];
        }
    }

    return (GhostModePhase){.state = CHASE, .duration_frames = -1};
}


void update_actor(Actor *a, uint32_t speed) {
    if (a->frames_to_pause > 0) {
        a->frames_to_pause--;
        return;
    }

    const uint32_t base = BIT_IS_SET(speed, 31); // 0 for slow mode, 1 for fast mode
    const uint8_t move = BIT_IS_SET(speed, (SPEED_IDX-1) - a->speed_idx) + base;

    for (int i = 0; i < move; i++) {
        switch (a->dir) {
            case UP: a->y -= 1;
                break;
            case RIGHT: a->x += 1;
                break;
            case DOWN: a->y += 1;
                break;
            case LEFT: a->x -= 1;
                break;
            default: break;
        }

        // Cornering / Snapping (Only for player)
        if (a->is_player) {
            if (a->dir == UP || a->dir == DOWN) {
                const int target_x = ((a->x + HALF) / TILE) * TILE;
                if (a->x < target_x) a->x++;
                else if (a->x > target_x) a->x--;
            } else if (a->dir == LEFT || a->dir == RIGHT) {
                const int target_y = ((a->y + HALF) / TILE) * TILE;
                if (a->y < target_y) a->y++;
                else if (a->y > target_y) a->y--;
            }
        }

        // don't advance mask index (actor moves 2 next time)
        if (move > 1 && is_centered(a->x, a->y)) {
            return;
        }
    }

    // Advance mask index
    a->speed_idx = (a->speed_idx + 1) % SPEED_IDX;
}

// Top-left positions (px, py), (gx, gy), square sprite size SPRITE.
static bool collide(float px, float py, float gx, float gy) {
    // Shrink each sprite's collision box to ~60% of sprite size.
    // This makes collisions feel less "cheap".
    constexpr float inset = SPRITE * 0.20f;      // 20% margin on each side
    constexpr float box   = SPRITE - 2.0f*inset; // effective collision size

    const float pLeft   = px + inset;
    const float pTop    = py + inset;
    const float pRight  = pLeft + box;
    const float pBottom = pTop + box;

    const float gLeft   = gx + inset;
    const float gTop    = gy + inset;
    const float gRight  = gLeft + box;
    const float gBottom = gTop + box;

    // AABB overlap: "any part overlaps" (not center equality).
    return (pLeft < gRight &&
            pRight > gLeft &&
            pTop < gBottom &&
            pBottom > gTop);
}

static void check_for_collisions(void) {
    const Actor *p = &game.player;
    for (int i = 0; i < NUM_GHOSTS; i++) {
        const Actor *g = &game.ghosts[i];
        if (collide(p->x, p->y, g->x, g->y)) {
            if (g->state == FRIGHTENED) {
                event_stack_push((Event){PLAYER_ATE_GHOST, g->id, g->x, g->y});
            } else if (!p->is_eaten) {
                event_stack_push((Event){GHOST_ATE_PLAYER, p->id, p->x, p->y});
            }
        }
    }
}

static void process_events(void) {
    Event event;
    while (event_stack_pop(&event)) {
        switch (event.type) {
            case PLAYER_ATE_DOT:
                game.maze[event.y][event.x] = TILE_EMPTY;
                game.dots_eaten++;
                game.dots_remaining--;
                game.player.frame_count += DOT_EAT_PAUSE;
                game.score += DOT_SCORE;
                break;
            case PLAYER_ATE_POWERUP:
                // TODO frighten ghosts
                game.maze[event.y][event.x] = TILE_EMPTY;
                game.dots_eaten++;
                game.dots_remaining--;
                game.ghosts_eaten = 0;
                game.is_fright_mode = true;
                game.player.frames_to_pause = POWER_EAT_PAUSE;
                game.score += POWER_SCORE;
                game.frightened_frames = game.level_spec.frightened_frames;
                game.frightened_flashes = game.level_spec.frightened_flashes;
                for (int i = 0; i < NUM_GHOSTS; i++) {
                    // TODO if a ghost was in the house when POWERUP was consumed,
                    // ghost should turn blue if frightened mode is still active
                    if (game.ghosts[i].state == SCATTER || game.ghosts[i].state == CHASE) {
                        game.ghosts[i].state = FRIGHTENED;
                        game.ghosts[i].reverse = true;
                    }
                }
                break;
            case GHOST_ATE_PLAYER:
                printf("ghost ate player\n");
                game.player.is_eaten = true;
                break;
            case PLAYER_ATE_GHOST:
                printf("player ate ghost at %d, %d\n", event.x, event.y);

                for (int i = 0; i < NUM_GHOSTS; i++) {
                    Actor *g = &game.ghosts[i];
                    if (g->id == event.id) {
                        g->state = EATEN;
                        game.score += get_ghost_score(game.ghosts_eaten);
                        game.score_pause = GHOST_EAT_PAUSE;
                        game.score_x = event.x;
                        game.score_y = event.y;
                        return; // only score one score at a time
                    }
                }
                break;
            case GHOST_STATE_CHANGE:
                printf("ghost state change\n");
                game.ghost_phase_index++;
                GhostModePhase phase = get_ghost_mode_phase(game.level, game.ghost_phase_index);
                game.ghost_phase_change = game.frame_count + phase.duration_frames;
                game.ghost_state = phase.state;
                for (int i = 0; i < NUM_GHOSTS; i++) {
                    if (game.ghosts[i].state == SCATTER || game.ghosts[i].state == CHASE) {
                        game.ghosts[i].state = game.ghost_state;
                        game.ghosts[i].reverse = true; // TODO do ghosts reverse on state change
                    }
                }
                break;
            case FRIGHT_MODE_END:
                printf("fright mode end\n");
                for (int i = 0; i < NUM_GHOSTS; i++) {
                    // TODO need game to keep track of ghost state
                    game.is_fright_mode = false;
                    game.frightened_flashes = 0;
                    game.frightened_frames = 0;
                    if (game.ghosts[i].state == FRIGHTENED) {
                        game.ghosts[i].state = CHASE;
                    }
                }
                break;
            case LEVEL_COMPLETE:
                printf("level over\n");
                game.is_paused = true;
                break;
        }
    }
}

void update_game(void) {
    if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_SPACE)) game.is_paused = !game.is_paused;
    if (IsKeyPressed(KEY_X)) game.draw_xrays = !game.draw_xrays;
    if (IsKeyPressed(KEY_T)) game.draw_targets = !game.draw_targets;
    if (IsKeyPressed(KEY_N)) {
        game.level++;
        init_maze(game.level);
    }
    if (IsKeyPressed(KEY_G)) {
        game.is_pacman = !game.is_pacman;
        game.level = 1;
        init_maze(game.level);
    }
    if (IsKeyPressed(KEY_C)) {
        game.ghost_state = CHASE;
        for (int i = 0; i < NUM_GHOSTS; i++) {
            if (game.ghosts[i].state == SCATTER || game.ghosts[i].state == CHASE) game.ghosts[i].state = CHASE;
        }
        printf("Changing to CHASE MODE\n");
    }

    if (IsKeyPressed(KEY_S)) {
        game.ghost_state = SCATTER;
        for (int i = 0; i < NUM_GHOSTS; i++) {
            if (game.ghosts[i].state == SCATTER || game.ghosts[i].state == CHASE) game.ghosts[i].state = SCATTER;
        }
        printf("Changing to SCATTER MODE\n");
    }


    if (game.is_paused) return;

    if (game.score_pause > 0) {
        game.score_pause--;
        if (game.score_pause == 0) {
            game.ghosts_eaten++;
        }
        return;
    }

    game.level_frame_count++;

    if (game.dots_remaining == 0) {
        event_stack_push((Event){LEVEL_COMPLETE, 0, 0, 0});
    }


    if (game.is_fright_mode) {
        game.frightened_frames--;
        if (game.frightened_frames == 0) {
            event_stack_push((Event){FRIGHT_MODE_END, 0, 0, 0});
        }
    } else {
        game.frame_count++;
        if (game.frame_count == game.ghost_phase_change) {
            event_stack_push((Event){GHOST_STATE_CHANGE, 0, 0, 0});
        }
    }

    check_for_collisions();
    process_events();
    update_ghosts();
    process_events();
    update_pacman();
    process_events();
}

void draw_game(void) {
    float zoom = game.screen_zoom;
    Camera2D text_camera = {
        .offset = {0, 2 * zoom}, // 2 pixel offset from top border
        .target = {0, 0},
        .rotation = 0.0f,
        .zoom = zoom
    };

    const Camera2D maze_camera = {
        .offset = {0, TOP_PADDING * TILE * zoom},
        .target = {0, 0},
        .rotation = 0.0f,
        .zoom = zoom
    };

    const Camera2D sprite_camera = {
        .offset = {0, (TOP_PADDING * TILE) * zoom},
        .target = {HALF, HALF},
        .rotation = 0.0f,
        .zoom = zoom
    };

    const Camera2D full_camera = {
        .offset = {0, 0 },
        .target = {HALF, HALF},
        .rotation = 0.0f,
        .zoom = zoom
    };

    //------------ DRAW TEXT ------------
    BeginMode2D(text_camera);
    draw_score_labels();
    EndMode2D();

    text_camera.offset.y = 3 * zoom;
    BeginMode2D(text_camera);
    draw_scores();
    EndMode2D();

    text_camera.offset.y = (GAME_HEIGHT - HALF) * zoom;
    BeginMode2D(text_camera);
    draw_lives();
    EndMode2D();

    //------------ DRAW MAZE ------------
    BeginMode2D(maze_camera);
    draw_maze();
    if (game.draw_xrays) draw_checkerboard();
    EndMode2D();

    //------------ DRAW SPRITES ------------
    BeginMode2D(sprite_camera);
    BeginShaderMode(game.chroma_shader); // shader makes BLACK transparent

    if (game.is_pacman) {
        draw_pacman(&game.player);
    } else {
        draw_ms_pacman(&game.player);
    }

    draw_ghosts();
    if (game.score_pause > 0) {
        draw_ghost_score(game.score_x, game.score_y, game.ghosts_eaten);
    }
    // game.paused = true;
    //draw_food();
    EndShaderMode();
    EndMode2D();

    BeginMode2D(full_camera);
    if (game.draw_targets) draw_targets();
    EndMode2D();
}
