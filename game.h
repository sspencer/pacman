//
// Created by Steve Spencer on 12/4/25.
//

#ifndef PACMAN_GAME_H
#define PACMAN_GAME_H

#include <raylib.h>
#include <stdint.h>

#define SCREEN_HEIGHT 36
#define SCREEN_WIDTH  28
#define GAME_WIDTH 28
#define GAME_HEIGHT 31
#define TOP_PADDING 3
#define SPRITE 16
#define TILE 8

#define FRIGHT_DURATION 6
#define BOUNCE_SPEED 0.1f
#define BOUNCE_AMPLITUDE 4.0f

#define DOT_MASK 103481868288
#define POWER_MASK 4359202964317896252

typedef enum {
    TILE_WALL,
    TILE_DOT,
    TILE_POWER,
    TILE_EMPTY,
    TILE_TUNNEL
} tile_t;

typedef enum {
    DIR_NORTH,
    DIR_SOUTH,
    DIR_EAST,
    DIR_WEST,
    DIR_COUNT
} dir_t;

typedef enum {
    GHOST_BLINKY,
    GHOST_INKY,
    GHOST_PINKY,
    GHOST_SUE,
    GHOST_COUNT,
} ghost_t;

typedef enum {
    STATE_SCATTER,
    STATE_CHASE,
    STATE_FRIGHTENED,
    STATE_EATEN,
    STATE_IN_HOUSE,
    STATE_LEAVING_HOUSE
} state_t;

// Red — Blinky
// Pink — Pinky
// Cyan / Light Blue — Inky
// Orange — Sue

typedef enum {
    SPRITE_ENTITY,
    SPRITE_EYES,
    SPRITE_BLUE,
    SPRITE_WHITE
} sprite_t;

typedef struct {
    float sprite_y;
    Vector2 start;
    dir_t start_dir;
    state_t start_state;
    Vector2 (*chase)(void);
    bool (*leave)(void);
    Vector2 (*scatter)(void);
} ghost_data_t;


typedef struct {
    Vector2 pos;
    Vector2 tile;
    Vector2 target; // TODO temp for ghost debugging
    float pixels_moved;
    float pixels_moved_in_dir;
    float bounce;
    dir_t dir;
    dir_t next_dir;
    bool eating_dot;
    Vector2 sprite[DIR_COUNT];
    uint8_t frame_count;
    uint8_t frame_index;
    int frames_to_pause; // when eating dots, pacman drops a frame or two
    sprite_t sprite_type;
    state_t state;

    // ghost behavior
    Vector2 (*chase)(void);
    bool (*leave)(void);
    Vector2 (*scatter)(void);
} entity_t;

typedef struct {
    uint8_t level;
    uint8_t maze[GAME_HEIGHT][GAME_WIDTH];
    int score;
    int dots_eaten;
    double level_time;
    double fright_time;
    bool paused;
    bool debug;
} game_t;

// Global world holds game state and resources so they don't need to be passed around
// Defined in main.c
typedef struct {
    game_t game;
    entity_t pacman;
    entity_t ghosts[GHOST_COUNT];
    Texture2D font;
    Texture2D texture;
    Image image;
} world_t;

extern world_t world;

void set_next_tile(entity_t *e, dir_t dir);
Vector2 get_next_tile(const entity_t *e, dir_t dir);
void move_entity(entity_t *e, float speed);
bool is_in_tunnel(Vector2 pos);

#endif //PACMAN_GAME_H