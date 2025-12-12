//
// Created by Steve Spencer on 12/4/25.
//

#ifndef PACMAN_GAME_H
#define PACMAN_GAME_H

#include <raylib.h>
#include <stdint.h>

#define DEBUG 0
#define SCREEN_HEIGHT 36
#define SCREEN_WIDTH  28
#define GAME_WIDTH 28
#define GAME_HEIGHT 31
#define TOP_PADDING 3
// sprite width and height
#define SPRITE 16
#define TILE 8
// NUDGE is always SIZE/2
#define TILE2 4
#define ZOOM 3
#define PIXEL (TILE * ZOOM)


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
    uint8_t pause_frames; // TODO what is this
    bool eating_dot;
    // *Timer player_eaten_timer
} player_data_t;

typedef struct {
    uint8_t ghost_id;
    uint8_t behavior; // TODO convert this
    ghost_t ghost_state;
    fright_t fright_state;
    Vector2 fright_sprite[FRIGHT_COUNT];
    Vector2 eye_sprite[DIR_COUNT];
    uint8_t color; // TODO color why is this needed?
    Vector2 target; // TODO temp for training?
    float bounce_time;
    uint8_t bounce; // TODO what is this
    float base_y;
    float pixels_moved_in_dir;
} ghost_data_t;
/*
struct ghost {
void (*update)(void* self, const uint8_t* map, int pacman_x, int pacman_y);
}
void blinky_update(void* self, const uint8_t*, int, int);
Entity ghosts[4] = {
   { .x = 14*8, .y = 11*8, .color = RED,   .update = blinky_update },
main loop:
for (int i = 0; i < 4; i++) {
ghosts[i].update(&ghosts[i], map, pacman_x, pacman_y);
}
example func:
void blinky_update(void* self, const uint8_t* map, int px, int py) {
    Entity* g = (Entity*)self;
    // Blinky: aggressive, chases Pac-Man directly
    int target_x = px;
    int target_y = py;

    choose_best_direction(g, map, target_x, target_y);
}
void make_ghosts_scared() {
    for (int i = 0; i < 4; i++) {
        ghosts[i].update = frightened_update;
        ghosts[i].is_scared = true;
    }
}

void make_ghosts_normal() {
    Entity* assignments[4] = {blinky_update, pinky_update, inky_update, clyde_update};
    for (int i = 0; i < 4; i++) {
        ghosts[i].update = assignments[i];
        ghosts[i].is_scared = false;
    }
}

// Recommended struct
typedef struct {
    int x, y;              // pixel position
    int tx, ty;            // tile position (optional)
    Direction dir;
    Direction next_dir;
    int speed;
    int color;
    bool scared;
    bool eyes_only;        // returning to house

    void (*update)(struct Entity* self,
                   const uint8_t* map,
                   int pacman_x, int pacman_y,
                   const struct Entity ghosts[4]);  // if Inky needs others
} Entity;
*/

typedef struct {
    float x, y; // pixel position
    int tx, ty; // tile position
    float pixels_moved;
    dir_t dir;
    dir_t next_dir;
    bool scared;
    bool eyes_only;
    bool eating_dot;
    int sprite_x[DIR_COUNT], sprite_y[DIR_COUNT];
    uint8_t frame_count;
    uint8_t frame_index;
} entity_t;

/*
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
    uint8_t frame_count;
    uint8_t frame;
    bool eaten;
    double speed_time;
    float speed_pixels;
    union {
        player_data_t player;
        ghost_data_t ghost;
    } entity_data;
} entity_t;
*/

typedef struct {
    uint8_t level;
    uint8_t maze[SCREEN_HEIGHT][SCREEN_WIDTH];
    int score;
    bool paused;
} game_t;

// Global world holds game state and resources so they don't need to be passed around
// Defined in main.c
typedef struct {
    game_t game;
    entity_t player;
    Texture2D game_texture;
    Image game_image;
} world_t;

extern Vector2 velocity[DIR_COUNT];

extern world_t world;

#endif //PACMAN_GAME_H