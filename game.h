//
// Created by Steve Spencer on 1/8/26.
//

#ifndef PACMAN_GAME_H
#define PACMAN_GAME_H

#include <stdint.h>
#include <stddef.h>
#include <raylib.h>
#include "event.h"

#define SCREEN_HEIGHT 36
#define SCREEN_WIDTH 28
#define GAME_WIDTH 28
#define GAME_HEIGHT 31
#define TOP_PADDING 3
#define SPRITE 16
#define TILE 8
#define HALF 4
#define NUM_GHOSTS 4
#define BOUNCE_SPEED 0.1f
#define BOUNCE_AMPLITUDE 4.0f

#define POWER_EAT_PAUSE 3
#define DOT_EAT_PAUSE 1
#define POWER_SCORE 50
#define DOT_SCORE 10
#define FRIGHTENED_FLASH_FRAMES 14

typedef enum
{
    TILE_WALL,
    TILE_DOOR,
    TILE_DOT,
    TILE_POWER,
    TILE_EMPTY,
    TILE_GHOST_WALL,
    TILE_TUNNEL
} Tile;

typedef enum
{
    UP,
    RIGHT,
    DOWN,
    LEFT,
    DIR_NONE
} Dir;

typedef enum
{
    BLINKY,
    INKY,
    PINKY,
    SUE
} GhostId;

typedef enum
{
    SCATTER,
    CHASE,
    FRIGHTENED,
    EATEN,
    IN_HOUSE,
    LEAVING_HOUSE
} GhostState;

typedef enum {
    SPD_PLAYER_NORMAL,
    SPD_PLAYER_POWER,

    SPD_GHOST_NORMAL,
    SPD_GHOST_FRIGHTENED,
    SPD_GHOST_TUNNEL,

    SPD_COUNT
} SpeedMode;

typedef struct {
    uint8_t level_min;
    uint8_t level_max; // inclusive
    uint32_t speeds[SPD_COUNT];
} SpeedTable;

typedef struct {
    GhostState state;
    int duration_frames; // -1.0f means indefinite
} GhostModePhase;

#define GHOST_MODE_PHASE_COUNT 8

typedef struct {
    uint8_t level_min;
    uint8_t level_max; // inclusive
    GhostModePhase phases[GHOST_MODE_PHASE_COUNT];
} GhostModePhaseTable;

typedef enum {
    BONUS_CHERRIES,
    BONUS_STRAWBERRY,
    BONUS_PEACH,
    BONUS_APPLE,
    BONUS_GRAPES,
    BONUS_GALAXIAN,
    BONUS_BELL,
    BONUS_KEY
} BonusSymbol;

typedef struct {
    uint8_t level_min;
    uint8_t level_max; // inclusive
    BonusSymbol bonus_symbol;
    uint16_t bonus_points;
    uint32_t pacman_speed;
    uint32_t pacman_fright_speed;
    uint32_t ghost_speed;
    uint32_t ghost_tunnel_speed;
    uint32_t ghost_fright_speed;
    uint8_t elroy_1_dots_left;
    uint32_t elroy_1_speed;
    uint8_t elroy_2_dots_left;
    uint32_t elroy_2_speed;
    int frightened_frames;
    int frightened_flashes;
} LevelSpecTable;

typedef struct
{
    int id;
    const char *name;
    int x, y;               // pixel location (not tile loc)
    int start_y;            // anchor for in-house bounce motion

    Dir dir;
    Dir req_dir; // Player: input dir; Ghost: unused

    int speed_idx; // Current index in the speed mask

    uint8_t frame_count; // sprite animation
    uint8_t frame_index; // sprite animation
    int pixels_moved;    // pixels moved by ghost (so it doesn't reversion direction)
    int frames_to_pause; // pacman slows when eating dots

    // bool is_frightened; // is_elroy1, is_elroy2
    // bool is_eaten;
    bool is_player;
    GhostState state;
    bool reverse; // ghosts reverse directions after CHASE / SCATTER state changes
    float bounce; // ghost (in house) bounce distance

    // Ghost Behavior
    Vector2 (*chase)(void);   // CHASE target
    bool (*leave)(void);      // time to LEAVE the house
    Vector2 (*scatter)(void); // SCATTER target
} Actor;

typedef struct
{
    Actor player;
    Actor ghosts[NUM_GHOSTS];
    uint8_t maze[GAME_HEIGHT][GAME_WIDTH];
    Texture2D sprite_texture;
    Texture2D font_texture;
    Shader chroma_shader;
    float screen_zoom;
    int score;
    int dots_eaten;
    int dots_remaining;
    GhostState ghost_state;
    uint32_t ghost_phase_change;
    uint32_t ghost_phase_index;
    uint32_t level_frame_count;
    uint32_t frame_count; // for SCATTER/CHASE mode (skipping is_frightened)
    uint8_t level;
    LevelSpecTable level_spec;
    int frightened_frames;
    int frightened_flashes;
    bool is_fright_mode; // flash cycle 28 frames (14 per color)
    bool is_pacman; // pacman game vs. ms. pacman
    bool is_paused;
    bool is_xray;
    EventStack event_stack;
} Game;

// globals
extern Game game;

void init_game(void);

void init_level(int);

void draw_game(void);

void update_game(void);

void update_actor(Actor *, uint32_t);

const char *dir_to_string(Dir);

bool is_centered(int x, int y);

LevelSpecTable get_level_spec(int level);

uint32_t get_ghost_speed(const Actor *);

uint32_t get_player_speed();

GhostModePhase get_ghost_mode_phase(int level, size_t index);

#endif // PACMAN_GAME_H
