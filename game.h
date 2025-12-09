//
// Created by Steve Spencer on 12/4/25.
//

#ifndef PACMAN_GAME_H
#define PACMAN_GAME_H

#include <raylib.h>

#define DEBUG 0
#define SCREEN_HEIGHT 36
#define SCREEN_WIDTH  28
#define GAME_WIDTH 28
#define GAME_HEIGHT 31
#define TOP_PADDING 3
#define ZOOM 3
#define SIZE 8
#define PIXEL (SIZE * ZOOM)

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
    TYPE_PLAYER,
    TYPE_GHOST
} entity_type_t;

typedef enum {
    NAME_PLAYER,
    NAME_BLINKY,
    NAME_INKY,
    NAME_PINKY,
    NAME_SUE,
    NAME_COUNT,
} entity_name_t;

typedef enum {
    GHOST_SCATTER,
    GHOST_CHASE,
    GHOST_FRIGHTENED,
    GHOST_EATEN,
    GHOST_IN_HOUSE,
    GHOST_LEAVING_HOUSE
} ghost_t;

typedef enum {
    FRIGHT_WHITE,
    FRIGHT_BLUE,
    FRIGHT_COUNT
} fright_t;

typedef struct {
    int score;
    int pause_frames;
    bool eating_dot;
    // *Timer player_eaten_timer
} player_data_t;

typedef struct {
    int ghost_id;
    int behavior; // TODO convert this
    ghost_t ghost_state;
    fright_t fright_state;
    Vector2 fright_sprite[FRIGHT_COUNT];
    Vector2 eye_sprite[DIR_COUNT];
    int color; // TODO color why is this needed?
    Vector2 target; // TODO temp for training?
    float bounce_time;
    int bounce;
    float base_y;
    float pixels_moved_in_dir;
} ghost_data_t;

typedef struct {
    entity_type_t type;
    entity_name_t name;
    Vector2 sprite[DIR_COUNT];
    dir_t dir;
    dir_t next_dir;
    Vector2 vel;
    Vector2 next_vel;
    Vector2 tile;
    Vector2 pixel;
    float pixels_moved;
    float width;
    float height;
    int frame_count;
    int frame;
    bool eaten;
    double speed_time;
    float speed_pixels;
    union {
        player_data_t player;
        ghost_data_t ghost;
    } entity_data;
} entity_t;

typedef struct {
    int level;
    int maze[SCREEN_HEIGHT][SCREEN_WIDTH];
    bool paused;
} game_t;

typedef struct {
    game_t *game;
    entity_t *player;
    Image *image;
    Texture2D texture;
} world_t;

#endif //PACMAN_GAME_H