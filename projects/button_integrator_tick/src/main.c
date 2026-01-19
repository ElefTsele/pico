#include <stdio.h>
#include "pico/stdlib.h"
#include "button.h"

int main(void)
{
    stdio_init_all();

    button_t btn;

    const uint BUTTON_GPIO = 15u;
    const uint32_t TICK_MS = 5u;
    const uint8_t MAX_TICKS = 6u; // 6 * 5ms = 30ms debounce

    button_status_t st = button_init(&btn, BUTTON_GPIO, true /*active low*/, MAX_TICKS);
    if (st != BUTTON_STATUS_OK) {
        printf("button_init failed: %d\n", (int)st);
        while (true) {
            tight_loop_contents();
        }
    }

    absolute_time_t next_tick = make_timeout_time_ms(TICK_MS);

    while (true) {

        if (time_reached(next_tick)) {
            next_tick = delayed_by_ms(next_tick, (int32_t)TICK_MS);
            button_update_tick(&btn);
        }

        button_event_t e = button_get_event(&btn);
        if (e == BUTTON_EVENT_PRESSED) {
            printf("PRESS\n");
        } else if (e == BUTTON_EVENT_RELEASED) {
            printf("RELEASE\n");
        }

        tight_loop_contents();
    }
}
