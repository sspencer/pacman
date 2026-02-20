#include "event.h"
#include "game.h"
#include <string.h>

void event_stack_init(void) {
    game.event_stack.top = -1;
}

bool event_stack_push(Event event) {
    if (event_stack_is_full()) {
        return false;
    }
    game.event_stack.events[++game.event_stack.top] = event;
    return true;
}

bool event_stack_pop(Event *event) {
    if (event_stack_is_empty()) {
        return false;
    }
    *event = game.event_stack.events[game.event_stack.top--];
    return true;
}

bool event_stack_is_empty(void) {
    return game.event_stack.top == -1;
}

bool event_stack_is_full(void) {
    return game.event_stack.top == MAX_EVENTS - 1;
}
