#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <stdint.h>

#include "hardware/gpio.h"

typedef enum {
    BUTTON_STATUS_OK = 0,
    BUTTON_STATUS_ERROR_NULL,
    BUTTON_STATUS_ERROR_INVALID_PARAM
} button_status_t;

typedef enum {
    BUTTON_EVENT_NONE = 0,
    BUTTON_EVENT_PRESSED,
    BUTTON_EVENT_RELEASED
} button_event_t;

typedef struct {
    uint gpio;
    bool active_low;

    uint8_t integrator;      // 0..max
    uint8_t max;             // debounce strength (ticks)

    bool stable_pressed;     // debounced state
    button_event_t event;    // latched edge event
} button_t;

/**
 * Initialize a button input.
 * - gpio: Pico GPIO number
 * - active_low: true if pressed reads 0 on the pin (button to GND)
 * - max: number of ticks required for stable press (debounce)
 */
button_status_t button_init(button_t *b, uint gpio, bool active_low, uint8_t max);

/**
 * Call this every tick (e.g. every 5 ms). Non-blocking.
 */
void button_update_tick(button_t *b);

/**
 * Returns the latched event and clears it.
 */
button_event_t button_get_event(button_t *b);

/**
 * Optional helper: current debounced state.
 */
bool button_is_pressed(const button_t *b);

#endif // BUTTON_H
 