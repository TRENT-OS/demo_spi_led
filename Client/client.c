#include <camkes.h>

int run(void) {
    const char * message = "Hensoldt Cyber!";
    while (1)
    {
        led_rpc_scroll_text(message);
    }
    return 0;
}
