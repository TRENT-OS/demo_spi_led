#include <camkes.h>

int run(void) {
    while (1)
    {
        led_rpc_scroll_text("HENSOLDT");
    }
    return 0;
}