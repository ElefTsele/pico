#include "button.h"

#include <stddef.h> // for NULL (optional)

static bool button_read_raw_pressed(const button_t *b)
{
    const bool level = gpio_get(b->gpio);            // 0/1
    return b->active_low ? (!level) : level;         // true when pressed
}

button_status_t button_init(button_t *b, uint gpio, bool active_low, uint8_t max)
{
    if (b == NULL) {
        return BUTTON_STATUS_ERROR_NULL;
    }

    if ((b == NULL) || (max == 0u)) {
        return BUTTON_STATUS_ERROR_INVALID_PARAM;
    }

    b->gpio = gpio;
    b->active_low = active_low;
    b->integrator = 0u;
    b->max = max;
    b->stable_pressed = false;
    b->event = BUTTON_EVENT_NONE;

    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_IN);

    if (active_low) {
        gpio_pull_up(gpio);     // button to GND
    } else {
        gpio_pull_down(gpio);   // button to 3V3
    }
    return BUTTON_STATUS_OK;
}

void button_update_tick(button_t *b)
{
    if (b == NULL) {
        return;
    }

    const bool pressed_raw = button_read_raw_pressed(b);

    // integrator debounce
    if (pressed_raw) {
        if (b->integrator < b->max) {
            b->integrator++;
        }
    } else {
        if (b->integrator > 0u) {
            b->integrator--;
        }
    }

    const bool pressed_stable  = (b->integrator >= b->max);
    const bool released_stable = (b->integrator == 0u);

    // edge events only
    if (pressed_stable && (b->stable_pressed == false)) {
        b->stable_pressed = true;
        b->event = BUTTON_EVENT_PRESSED;
    } else if (released_stable && (b->stable_pressed == true)) {
        b->stable_pressed = false;
        b->event = BUTTON_EVENT_RELEASED;
    } else {
        // keep latched event until consumed
    }
}

button_event_t button_get_event(button_t *b)
{
    if (b == NULL) {
        return BUTTON_EVENT_NONE;
    }

    const button_event_t e = b->event;
    b->event = BUTTON_EVENT_NONE;
    return e;
}

bool button_is_pressed(const button_t *b)
{
    if (b == NULL) {
        return false;
    }
    return b->stable_pressed;
}
