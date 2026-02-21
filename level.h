//
// Created by Steve Spencer on 2/11/26.
//

#ifndef PACMAN_LEVEL_H
#define PACMAN_LEVEL_H

#include "game.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define BIT_IS_SET(value, n) ((((value) >> (n)) & 1u) != 0)

// Arcade was hardware clocked to 60.606061 frames per second,
// max speed (100%) worked out to 75.75757625 pixels per second.
// MASK_80 (80%) advances 1 pixel per frame, approx 60pps
#define SPEED_IDX (16)

// Most significant bit determines if speed is in slow mode of fast mode
// Speed is determined by least significant 16 bits.
//   0: slow mode - 0 is 0 pixels, 1 is 1 pixel
//   1: fast mode - 1 is 1 pixel, 2 is 2 pixels
//
// Arcade game was clocked at 60.6060606 frames per second
//   100% speed is 75.75757625 pixels per second
//    80% speed is 60.6060606 pixels per second (1 pixel per frame)
//
#define SPEED_40 0x0000AAAA
#define SPEED_45 0x0000B555
#define SPEED_50 0x0000B6B6
#define SPEED_55 0x0000B6B7
#define SPEED_60 0x0000EEEE
#define SPEED_65 0x0000EEEF
#define SPEED_75 0x0000FFFD
#define SPEED_80 0x0000FFFF
#define SPEED_85 0x80000001  // start of fast mode
#define SPEED_90 0x80000101
#define SPEED_95 0x80000421
#define SPEED_100 0x80001111
#define SPEED_105 0x80002492

#define SPEED_NA (0u)
#define FRIGHT_TIME_NA (-1)
#define FLASHES_NA (-1)
#define SECONDS_TO_FRAMES(x) ((int)((x) * 60.606061))

static constexpr LevelSpecTable pacman_level_table[] = {
    {.level_min = 1, .level_max = 1, .bonus_symbol = BONUS_CHERRIES, .bonus_points = 100,
        .pacman_speed = SPEED_80, .ghost_speed = SPEED_75, .ghost_tunnel_speed = SPEED_40,
        .elroy_1_dots_left = 20, .elroy_1_speed = SPEED_80, .elroy_2_dots_left = 10, .elroy_2_speed = SPEED_85,
        .pacman_fright_speed = SPEED_90, .ghost_fright_speed = SPEED_50,
        .frightened_frames = SECONDS_TO_FRAMES(6), .frightened_flashes = 5},

    {.level_min = 2, .level_max = 2, .bonus_symbol = BONUS_STRAWBERRY, .bonus_points = 300,
        .pacman_speed = SPEED_90, .ghost_speed = SPEED_85, .ghost_tunnel_speed = SPEED_45,
        .elroy_1_dots_left = 30, .elroy_1_speed = SPEED_90, .elroy_2_dots_left = 15, .elroy_2_speed = SPEED_95,
        .pacman_fright_speed = SPEED_95, .ghost_fright_speed = SPEED_55,
        .frightened_frames = SECONDS_TO_FRAMES(5), .frightened_flashes = 5},

    {.level_min = 3, .level_max = 3, .bonus_symbol = BONUS_PEACH, .bonus_points = 500,
        .pacman_speed = SPEED_90, .ghost_speed = SPEED_85, .ghost_tunnel_speed = SPEED_45,
        .elroy_1_dots_left = 40, .elroy_1_speed = SPEED_90, .elroy_2_dots_left = 20, .elroy_2_speed = SPEED_95,
        .pacman_fright_speed = SPEED_95, .ghost_fright_speed = SPEED_55,
        .frightened_frames = SECONDS_TO_FRAMES(4), .frightened_flashes = 5},

    {.level_min = 4, .level_max = 4, .bonus_symbol = BONUS_PEACH, .bonus_points = 500,
        .pacman_speed = SPEED_90, .ghost_speed = SPEED_85, .ghost_tunnel_speed = SPEED_45,
        .elroy_1_dots_left = 40, .elroy_1_speed = SPEED_90, .elroy_2_dots_left = 20, .elroy_2_speed = SPEED_95,
        .pacman_fright_speed = SPEED_95, .ghost_fright_speed = SPEED_55,
        .frightened_frames = SECONDS_TO_FRAMES(3), .frightened_flashes = 5},

    {.level_min = 5, .level_max = 5, .bonus_symbol = BONUS_APPLE, .bonus_points = 700,
        .pacman_speed = SPEED_100, .ghost_speed = SPEED_95, .ghost_tunnel_speed = SPEED_50,
        .elroy_1_dots_left = 40, .elroy_1_speed = SPEED_100, .elroy_2_dots_left = 20, .elroy_2_speed = SPEED_105,
        .pacman_fright_speed = SPEED_100, .ghost_fright_speed = SPEED_60,
        .frightened_frames = SECONDS_TO_FRAMES(2), .frightened_flashes = 5},

    {.level_min = 6, .level_max = 6, .bonus_symbol = BONUS_APPLE, .bonus_points = 700,
        .pacman_speed = SPEED_100, .ghost_speed = SPEED_95, .ghost_tunnel_speed = SPEED_50,
        .elroy_1_dots_left = 50, .elroy_1_speed = SPEED_100, .elroy_2_dots_left = 25, .elroy_2_speed = SPEED_105,
        .pacman_fright_speed = SPEED_100, .ghost_fright_speed = SPEED_60,
        .frightened_frames = SECONDS_TO_FRAMES(5), .frightened_flashes = 5},

    {.level_min = 7, .level_max = 8, .bonus_symbol = BONUS_GRAPES, .bonus_points = 1000,
        .pacman_speed = SPEED_100, .ghost_speed = SPEED_95, .ghost_tunnel_speed = SPEED_50,
        .elroy_1_dots_left = 50, .elroy_1_speed = SPEED_100, .elroy_2_dots_left = 25, .elroy_2_speed = SPEED_105,
        .pacman_fright_speed = SPEED_100, .ghost_fright_speed = SPEED_60,
        .frightened_frames = SECONDS_TO_FRAMES(2), .frightened_flashes = 5},

    {.level_min = 9, .level_max = 9, .bonus_symbol = BONUS_GALAXIAN, .bonus_points = 2000,
        .pacman_speed = SPEED_100, .ghost_speed = SPEED_95, .ghost_tunnel_speed = SPEED_50,
        .elroy_1_dots_left = 60, .elroy_1_speed = SPEED_100, .elroy_2_dots_left = 30, .elroy_2_speed = SPEED_105,
        .pacman_fright_speed = SPEED_100, .ghost_fright_speed = SPEED_60,
        .frightened_frames = SECONDS_TO_FRAMES(1), .frightened_flashes = 3},

    {.level_min = 10, .level_max = 10, .bonus_symbol = BONUS_GALAXIAN, .bonus_points = 2000,
        .pacman_speed = SPEED_100, .ghost_speed = SPEED_95, .ghost_tunnel_speed = SPEED_50,
        .elroy_1_dots_left = 60, .elroy_1_speed = SPEED_100, .elroy_2_dots_left = 30, .elroy_2_speed = SPEED_105,
        .pacman_fright_speed = SPEED_100, .ghost_fright_speed = SPEED_60,
        .frightened_frames = SECONDS_TO_FRAMES(5), .frightened_flashes = 5},

    {.level_min = 11, .level_max = 11, .bonus_symbol = BONUS_BELL, .bonus_points = 3000,
        .pacman_speed = SPEED_100, .ghost_speed = SPEED_95, .ghost_tunnel_speed = SPEED_50,
        .elroy_1_dots_left = 60, .elroy_1_speed = SPEED_100, .elroy_2_dots_left = 30, .elroy_2_speed = SPEED_105,
        .pacman_fright_speed = SPEED_100, .ghost_fright_speed = SPEED_60,
        .frightened_frames = SECONDS_TO_FRAMES(2), .frightened_flashes = 5},

    {.level_min = 12, .level_max = 12, .bonus_symbol = BONUS_BELL, .bonus_points = 3000,
        .pacman_speed = SPEED_100, .ghost_speed = SPEED_95, .ghost_tunnel_speed = SPEED_50,
        .elroy_1_dots_left = 80, .elroy_1_speed = SPEED_100, .elroy_2_dots_left = 40, .elroy_2_speed = SPEED_105,
        .pacman_fright_speed = SPEED_100, .ghost_fright_speed = SPEED_60,
        .frightened_frames = SECONDS_TO_FRAMES(1), .frightened_flashes = 3},

    {.level_min = 13, .level_max = 13, .bonus_symbol = BONUS_KEY, .bonus_points = 5000,
        .pacman_speed = SPEED_100, .ghost_speed = SPEED_95, .ghost_tunnel_speed = SPEED_50,
        .elroy_1_dots_left = 80, .elroy_1_speed = SPEED_100, .elroy_2_dots_left = 40, .elroy_2_speed = SPEED_105,
        .pacman_fright_speed = SPEED_100, .ghost_fright_speed = SPEED_60,
        .frightened_frames = SECONDS_TO_FRAMES(1), .frightened_flashes = 3},

    {.level_min = 14, .level_max = 14, .bonus_symbol = BONUS_KEY, .bonus_points = 5000,
        .pacman_speed = SPEED_100, .ghost_speed = SPEED_95, .ghost_tunnel_speed = SPEED_50,
        .elroy_1_dots_left = 80, .elroy_1_speed = SPEED_100, .elroy_2_dots_left = 40, .elroy_2_speed = SPEED_105,
        .pacman_fright_speed = SPEED_100, .ghost_fright_speed = SPEED_60,
        .frightened_frames = SECONDS_TO_FRAMES(3), .frightened_flashes = 5},

    {.level_min = 15, .level_max = 16, .bonus_symbol = BONUS_KEY, .bonus_points = 5000,
        .pacman_speed = SPEED_100, .ghost_speed = SPEED_95, .ghost_tunnel_speed = SPEED_50,
        .elroy_1_dots_left = 100, .elroy_1_speed = SPEED_100, .elroy_2_dots_left = 50, .elroy_2_speed = SPEED_105,
        .pacman_fright_speed = SPEED_100, .ghost_fright_speed = SPEED_60,
        .frightened_frames = SECONDS_TO_FRAMES(1), .frightened_flashes = 3},

    {.level_min = 17, .level_max = 17, .bonus_symbol = BONUS_KEY, .bonus_points = 5000,
        .pacman_speed = SPEED_100, .ghost_speed = SPEED_95, .ghost_tunnel_speed = SPEED_50,
        .elroy_1_dots_left = 100, .elroy_1_speed = SPEED_100, .elroy_2_dots_left = 50, .elroy_2_speed = SPEED_105,
        .pacman_fright_speed = SPEED_NA, .ghost_fright_speed = SPEED_NA,
        .frightened_frames = FRIGHT_TIME_NA, .frightened_flashes = FLASHES_NA},

    {.level_min = 18, .level_max = 18, .bonus_symbol = BONUS_KEY, .bonus_points = 5000,
        .pacman_speed = SPEED_100, .ghost_speed = SPEED_95, .ghost_tunnel_speed = SPEED_50,
        .elroy_1_dots_left = 100, .elroy_1_speed = SPEED_100, .elroy_2_dots_left = 50, .elroy_2_speed = SPEED_105,
        .pacman_fright_speed = SPEED_100, .ghost_fright_speed = SPEED_60,
        .frightened_frames = SECONDS_TO_FRAMES(1), .frightened_flashes = 3},

    {.level_min = 19, .level_max = 20, .bonus_symbol = BONUS_KEY, .bonus_points = 5000,
        .pacman_speed = SPEED_100, .ghost_speed = SPEED_95, .ghost_tunnel_speed = SPEED_50,
        .elroy_1_dots_left = 120, .elroy_1_speed = SPEED_100, .elroy_2_dots_left = 60, .elroy_2_speed = SPEED_105,
        .pacman_fright_speed = SPEED_NA, .ghost_fright_speed = SPEED_NA,
        .frightened_frames = FRIGHT_TIME_NA, .frightened_flashes = FLASHES_NA},

    {.level_min = 21, .level_max = 255, .bonus_symbol = BONUS_KEY, .bonus_points = 5000,
        .pacman_speed = SPEED_90, .ghost_speed = SPEED_95, .ghost_tunnel_speed = SPEED_50,
        .elroy_1_dots_left = 120, .elroy_1_speed = SPEED_100, .elroy_2_dots_left = 60, .elroy_2_speed = SPEED_105,
        .pacman_fright_speed = SPEED_NA, .ghost_fright_speed = SPEED_NA,
        .frightened_frames = FRIGHT_TIME_NA, .frightened_flashes = FLASHES_NA},
};

#define LEVEL_SPEC_TABLE_SIZE ARRAY_SIZE(pacman_level_table)

static constexpr GhostModePhaseTable ghost_mode_phase_table[] = {
    // Level 1
    {
        .level_min = 1,
        .level_max = 1,
        .phases = {
            [0] = {.state = SCATTER, .duration_frames = SECONDS_TO_FRAMES(7)},
            [1] = {.state = CHASE, .duration_frames = SECONDS_TO_FRAMES(20)},
            [2] = {.state = SCATTER, .duration_frames = SECONDS_TO_FRAMES(7)},
            [3] = {.state = CHASE, .duration_frames = SECONDS_TO_FRAMES(20)},
            [4] = {.state = SCATTER, .duration_frames = SECONDS_TO_FRAMES(5)},
            [5] = {.state = CHASE, .duration_frames = SECONDS_TO_FRAMES(20)},
            [6] = {.state = SCATTER, .duration_frames = SECONDS_TO_FRAMES(5)},
            [7] = {.state = CHASE, .duration_frames = -1},
        }
    },

    // Levels 2-4
    {
        .level_min = 2,
        .level_max = 4,
        .phases = {
            [0] = {.state = SCATTER, .duration_frames = SECONDS_TO_FRAMES(7)},
            [1] = {.state = CHASE, .duration_frames = SECONDS_TO_FRAMES(20)},
            [2] = {.state = SCATTER, .duration_frames = SECONDS_TO_FRAMES(7)},
            [3] = {.state = CHASE, .duration_frames = SECONDS_TO_FRAMES(20)},
            [4] = {.state = SCATTER, .duration_frames = SECONDS_TO_FRAMES(5)},
            [5] = {.state = CHASE, .duration_frames = SECONDS_TO_FRAMES(1033)},
            [6] = {.state = SCATTER, .duration_frames = 1}, // 1 frame
            [7] = {.state = CHASE, .duration_frames = -1}, // indefinite
        }
    },

    // Levels 5+
    {
        .level_min = 5,
        .level_max = 255,
        .phases = {
            [0] = {.state = SCATTER, .duration_frames = SECONDS_TO_FRAMES(5.0)},
            [1] = {.state = CHASE, .duration_frames = SECONDS_TO_FRAMES(20)},
            [2] = {.state = SCATTER, .duration_frames = SECONDS_TO_FRAMES(5)},
            [3] = {.state = CHASE, .duration_frames = SECONDS_TO_FRAMES(20)},
            [4] = {.state = SCATTER, .duration_frames = SECONDS_TO_FRAMES(5)},
            [5] = {.state = CHASE, .duration_frames = SECONDS_TO_FRAMES(1037)},
            [6] = {.state = SCATTER, .duration_frames = 1}, // 1 frame
            [7] = {.state = CHASE, .duration_frames = -1}, // indefinite
        }
    }
};

#define GHOST_MODE_PHASE_TABLE_SIZE ARRAY_SIZE(ghost_mode_phase_table)

#endif //PACMAN_LEVEL_H
