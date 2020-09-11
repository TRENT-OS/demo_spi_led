#include <camkes.h>

int run(void) {
    const char * message = "Hello";
    /*led_rpc_scroll_text(message);
    led_rpc_scroll_text(message);
    led_rpc_display_char('E');
    led_rpc_clear_display();
    led_rpc_display_char_on_device('L',3);*/
    while (1)
    {
        led_rpc_scroll_text(message);
    }
    return 0;
}