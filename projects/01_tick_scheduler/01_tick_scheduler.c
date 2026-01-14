#include <stdio.h>
#include "pico/stdlib.h"

/*
    Tick Scheduler Demo (Bare-metal, non-blocking "superloop")

    Concept:
    - Instead of using sleep/delay inside the main loop, we run periodic tasks
      based on time checks ("ticks").
    - This is a cooperative scheduler: tasks must be short and non-blocking,
      otherwise they delay everything else.
*/

static void led_init(uint led_pin)
{
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
}

static void led_toggle(uint pin)
{
    gpio_xor_mask(1u << pin);
}

static inline uint32_t now_ms(void)
{
    // get_absolute_time() -> returns an internal time type (absolute_time_t)
    // to_ms_since_boot()  -> converts it to milliseconds since system boot
    return to_ms_since_boot(get_absolute_time());
}

int main(void)
{
    stdio_init_all();
    sleep_ms(500); 

    const uint LED_PIN = 25u;
    led_init(LED_PIN);

    const uint32_t BUTTON_TICK_MS = 5u;     // 5ms tick for button
    const uint32_t LED_TICK_MS    = 250u;   // LED toggles every 250 ms

    uint32_t next_button_ms = now_ms();
    uint32_t next_led_ms    = now_ms();

    while (true)
    {
        uint32_t t = now_ms();

        /*
            Task 1: Button tick (every 5ms)
            - In the future, we call: button_update_tick(&btn);
            - This should be fast (microseconds), no blocking allowed.
        */
       // Signed compare to handle timer wrap-around safely
        if ((int32_t)(t - next_button_ms) >= 0)
        {
            next_button_ms += BUTTON_TICK_MS;

            // button_update_tick(&btn);
        }

        /*
            Task 2: LED tick
        */
        if ((int32_t)(t - next_led_ms) >= 0)
        {
            next_led_ms += LED_TICK_MS;
            led_toggle(LED_PIN);
        }

        /*
            Other tasks could run here continuously:
            IMPORTANT:
            - Keep tasks short (non-blocking), otherwise the whole loop stalls.
        */

        // -- Pico SDK helper for tight busy-loops (can improve debug/power/housekeeping)
        tight_loop_contents();
    }
}
