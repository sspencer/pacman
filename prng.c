//
// Created by Steve Spencer on 2/20/26.
//

#include "prng.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// Simulated 8 KB ROM (first 8192 bytes)
#define ROM_SIZE 8192
unsigned char rom[ROM_SIZE];

unsigned int prng_index = 0;  // Global PRNG state (shared by all ghosts)

// Advance PRNG and get next "random" byte from simulated ROM
unsigned char get_prng_byte(void) {
   prng_index = (prng_index * 5ULL + 1) % ROM_SIZE;
   return rom[prng_index];
}

void init_prng(void) {
   // Seed for reproducibility (simulating fixed ROM patterns)
   srand(42);

   // Fill simulated ROM with pseudo-random bytes
   for (int i = 0; i < ROM_SIZE; i++) {
      rom[i] = (unsigned char)rand();
   }
}

// Reset PRNG for a new "level"
void reset_prng() {
   prng_index = 0;
}

// Get preferred direction from lowest 2 bits of the PRNG byte
Dir get_preferred_direction() {
   unsigned char byte = get_prng_byte();
   return (Dir)(byte & 0x03);  // lowest 2 bits: 0=UP, 1=RIGHT, 2=DOWN, 3=LEFT
}

// Clockwise next direction: UP -> RIGHT -> DOWN -> LEFT
Dir clockwise_next(Dir d) {
   return (Dir)((d + 1) % DIR_COUNT);
}

// Simulate choosing any of the allowed directions (including reverse)
// current_dir  = ghost's current travel direction
// allowed_dirs = bitmask: bit 0 = right allowed, bit 1 = down, etc. (1=allowed, 0=wall)
Dir choose_random_direction(Dir current_dir, const unsigned int allowed_dirs) {
   const Dir preferred = get_preferred_direction();
   Dir test = preferred;

   for (int attempts = 0; attempts < DIR_COUNT; attempts++) {
      // Check if this direction is allowed (not wall) and not reverse
      if (allowed_dirs & (1u << test)) {
         return test;
      }
      // Pivot clockwise to next preference
      test = clockwise_next(test);
   }

   // Should never reach here if at least one valid direction exists
   return current_dir;  // fallback
}

// Simulate choosing next direction at an intersection (reverse won't be choosen)
// current_dir  = ghost's current travel direction
// allowed_dirs = bitmask: bit 0 = right allowed, bit 1 = down, etc. (1=allowed, 0=wall)
Dir choose_frightened_direction(Dir current_dir, const unsigned int allowed_dirs) {
   const Dir preferred = get_preferred_direction();
   Dir test = preferred;
   const Dir reverse = opposite_dir[current_dir];

   for (int attempts = 0; attempts < DIR_COUNT; attempts++) {
       // Check if this direction is allowed (not wall) and not reverse
       if ((allowed_dirs & (1u << test)) && test != reverse) {
           return test;
       }
       // Pivot clockwise to next preference
       test = clockwise_next(test);
   }

   // Should never reach here if at least one valid direction exists
   return current_dir;  // fallback
}

/*
int main(void) {
   // Seed for reproducibility (simulating fixed ROM patterns)
   srand(42);

   // Fill simulated ROM with pseudo-random bytes
   for (int i = 0; i < ROM_SIZE; i++) {
       rom[i] = (unsigned char)rand();
   }

   printf("Simulated Pac-Man frightened ghost direction selection\n");
   printf("PRNG: index = (index * 5 + 1) %% %d\n", ROM_SIZE);
   printf("Direction from lowest 2 bits of rom[index]\n\n");

   // Reset PRNG for a new "level"
   prng_index = 0;
   printf("Starting new level/life — PRNG index reset to 0\n\n");

   // Example: simulate several direction choices for one ghost
   Direction current = DIR_RIGHT;           // suppose ghost is moving RIGHT
   unsigned int allowed = 0b1011;           // e.g. right, down, up allowed (no left = wall)

   for (int i = 1; i <= 12; i++) {
       Direction next = choose_frightened_direction(current, allowed);
       printf("Choice #%2d: preferred → %s  |  chose → %s\n",
              i, dir_names[get_preferred_direction()], dir_names[next]);
       current = next;  // pretend it moved that way
   }

   printf("\nNote: In real game, allowed directions come from maze tiles.\n");
   printf("All ghosts share the same PRNG state — next ghost would continue sequence.\n");

   return 0;
}
*/