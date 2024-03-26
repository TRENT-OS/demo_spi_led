/*
 * Copyright (C) 2020-2024, HENSOLDT Cyber GmbH
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * For commercial licensing, contact: info.cyber@hensoldt.net
 */

#include <camkes.h>

int run(void) {
    const char * message = "Hensoldt Cyber!";
    while (1)
    {
        led_rpc_scroll_text(message);
    }
    return 0;
}
