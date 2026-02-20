#ifndef PACMAN_EVENT_H
#define PACMAN_EVENT_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    GHOST_ATE_PLAYER,
    PLAYER_ATE_GHOST,
    PLAYER_ATE_DOT,
    PLAYER_ATE_POWERUP,
    GHOST_STATE_CHANGE,
    FRIGHT_MODE_END,
    LEVEL_COMPLETE,

} EventType;

typedef struct {
    EventType type;
    int id;
    int x, y;
} Event;

#define MAX_EVENTS 256

typedef struct {
    Event events[MAX_EVENTS];
    int top; // Using as a stack as requested: "push and pop events off of a stack"
} EventStack;

void event_stack_init(void);
bool event_stack_push(Event event);
bool event_stack_pop(Event *event);
bool event_stack_is_empty(void);
bool event_stack_is_full(void);

#endif //PACMAN_EVENT_H
