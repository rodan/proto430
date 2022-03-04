
#include <stdio.h>
#include <string.h>
#include "glue.h"
#include "version.h"
#include "ui.h"

extern uart_descriptor bc;

static const char menu_str[]="\
 available commands:\r\n\r\n\
\033[33;1m?\033[0m  - show menu\r\n\
\033[33;1m5v on/off\033[0m  - control 5v rail\r\n";

void display_menu(void)
{
    display_version();
    uart_print(&bc, menu_str);
}

void display_version(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];

    uart_print(&bc, "vfd demo b");
    uart_print(&bc, _utoa(sconv, BUILD));
    uart_print(&bc, "\r\n");
}

#define PARSER_CNT 16

void parse_user_input(void)
{
#if defined UART_RX_USES_RINGBUF
    struct ringbuf *rbr = uart_get_rx_ringbuf(&bc);
    uint8_t rx;
    uint8_t c = 0;
    char input[PARSER_CNT];

    memset(input, 0, PARSER_CNT);

    // read the entire ringbuffer
    while (ringbuf_get(rbr, &rx)) {
        if (c < PARSER_CNT-1) {
            input[c] = rx;
        }
        c++;
    }
#else
    char *input = uart_get_rx_buf(&bc);
#endif
    char f = input[0];
    //char itoa_buf[CONV_BASE_10_BUF_SZ];
    //uint32_t in=0;

    if (f == '?') {
        display_menu();
    } else if (strstr(input, "5v on")) {
        rail_5v_on;
    } else if (strstr(input, "5v off")) {
        rail_5v_off;
    } else {
        //uart_tx_str("\r\n", 2);
    }
}
