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
} ghost_name_t;

typedef enum {
    GHOST_SCATTER,
    GHOST_CHASE,
    GHOST_FRIGHTENED,
    GHOST_EATEN,
    GHOST_IN_HOUSE,
    GHOST_LEAVING_HOUSE
} ghost_state_t;

typedef enum {
    FRIGHT_WHITE,
    FRIGHT_BLUE,
    FRIGHT_COUNT
} fright_state_t;


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
    Vector2 sprite;
    Vector2 start;
    dir_t start_dir;
    ghost_state_t start_state;
    Vector2 (*chase)(void);
    bool (*leave)(void);
    Vector2 (*scatter)(void);
} ghost_data_t;


typedef struct {
    //float x, y; // pixel position
    Vector2 pos;
    //int tx, ty; // tile position
    Vector2 tile;
    float pixels_moved;
    dir_t dir;
    dir_t next_dir;
    bool scared;
    bool eyes_only;
    bool eating_dot;
    int sprite_x[DIR_COUNT], sprite_y[DIR_COUNT];
    uint8_t frame_count;
    uint8_t frame_index;
    int frames_to_pause; // when eating dots, pacman drops a frame or two

    // ghosts
    ghost_state_t state;
    float pixels_moved_in_dir;
    Vector2 (*chase)(void); // ghosts only
    bool (*leave)(void); // ghosts only
    Vector2 (*scatter)(void); // ghosts only
} entity_t;

typedef struct {
    uint8_t level;
    uint8_t maze[GAME_HEIGHT][GAME_WIDTH];
    int score;
    int dots_eaten;
    double level_time;
    bool paused;
    bool debug;
} game_t;

// Global world holds game state and resources so they don't need to be passed around
// Defined in main.c
typedef struct {
    game_t game;
    entity_t pacman;
    entity_t ghosts[GHOST_COUNT];
    Texture2D game_texture;
    Image game_image;
} world_t;

extern world_t world;

void set_next_tile(entity_t *e, dir_t dir);
Vector2 get_next_tile(const entity_t *e, dir_t dir);
void move_entity(entity_t *e, float speed);
bool is_in_tunnel(Vector2 pos);

#endif //PACMAN_GAME_H