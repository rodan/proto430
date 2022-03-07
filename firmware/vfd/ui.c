
#include <stdio.h>
#include <string.h>
#include "glue.h"
#include "version.h"
#include "timer_a2.h"
#include "eeram_48l_extra.h"
#include "vfd_extra.h"
#include "rtca_now.h"
#include "ui.h"

extern uart_descriptor bc;

static const char menu_str[]="\
 available commands:\r\n\r\n\
\033[33;1m?\033[0m  - show menu\r\n\
\033[33;1m5v on/off\033[0m  - control 5v rail\r\n\
\033[33;1mtr/tw\033[0m  - test rtc\r\n\
\033[33;1msr/sw\033[0m  - test eeram\r\n";

//static uint8_t test_str[]={0x1b, '@', 'H', 'e', 'l', 'l', 'o', 0x1f, 0x28, 'g', 0x40, };

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

void ui_vfd_refresh(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];
    uint16_t rnd;

    rnd = 146 - (rand() % 20);

    vfd_cmd_clear(&vfdd);
    //vfd_tx_str(&vfdd, (char *) test_str, 7);
    //vfd_print(&vfdd, "Hello");
    vfd_us_cmd(&vfdd, 'g', 0x40);
    vfd_tx(&vfdd, 2);
    vfd_tx(&vfdd, 2);
    vfd_print(&vfdd, " ");
    vfd_print(&vfdd, _utoa(sconv, rnd));
    vfd_print(&vfdd, ".1 ");
    vfd_tx(&vfdd, 0xe6);
    vfd_print(&vfdd, "S ");

    rnd = 25 - (rand() % 3);
    vfd_us_cmd(&vfdd, 'g', 0x40);
    vfd_tx(&vfdd, 1);
    vfd_tx(&vfdd, 1);
    vfd_print(&vfdd, "                    ");
    vfd_print(&vfdd, "   ");
    vfd_print(&vfdd, _utoa(sconv, rnd));
    vfd_tx(&vfdd, 0xf8);
    vfd_print(&vfdd, "C   ");

#ifdef CONFIG_DS3231
    struct ts t;

    DS3231_get(I2C_BASE_ADDR, &t);

    vfd_print(&vfdd, prepend_padding(sconv, _utoa(sconv, t.hour), PAD_ZEROES, 2));
    vfd_print(&vfdd, ":");
    vfd_print(&vfdd, prepend_padding(sconv, _utoa(sconv, t.min), PAD_ZEROES, 2));
    vfd_print(&vfdd, ":");
    vfd_print(&vfdd, prepend_padding(sconv, _utoa(sconv, t.sec), PAD_ZEROES, 2));
#endif
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
    //uint32_t in=0;
    uint8_t val = 0;
#ifdef CONFIG_DS3231
    struct ts t;
    char sconv[CONV_BASE_10_BUF_SZ];
#endif

    if (f == '?') {
        display_menu();
    } else if (strstr(input, "5v on")) {
        rail_5v_on;
        //vfd_cmd_init(&vfdd);
    } else if (strstr(input, "5v off")) {
        timer_a2_set_trigger_slot(SCHEDULE_VFD_REFRESH, 0, TIMER_A2_EVENT_DISABLE);
        rail_5v_off;
    } else if (strstr(input, "vfd")) {
        ui_vfd_refresh();
        timer_a2_set_trigger_slot(SCHEDULE_VFD_REFRESH, systime()+100, TIMER_A2_EVENT_ENABLE);
    } else if (strstr(input, "clr")) {
        timer_a2_set_trigger_slot(SCHEDULE_VFD_REFRESH, 0, TIMER_A2_EVENT_DISABLE);
        vfd_cmd_clear(&vfdd);
    } else if (strstr(input, "sr")) {
        EERAM_48L_read_streg(&spid_eeram, &val);
    } else if (strstr(input, "sw")) {
        EERAM_48L_write_streg(&spid_eeram, EERAM_48L512_SR_ASE );
#ifdef CONFIG_DS3231
    } else if (strstr(input, "tr")) {
        DS3231_get(I2C_BASE_ADDR, &t);

        uart_print(&bc, _utoa(sconv, t.year));
        uart_print(&bc, ".");
        uart_print(&bc, prepend_padding(sconv, _utoa(sconv, t.mon), PAD_ZEROES, 2));
        uart_print(&bc, ".");
        uart_print(&bc, prepend_padding(sconv, _utoa(sconv, t.mday), PAD_ZEROES, 2));
        uart_print(&bc, " ");
        uart_print(&bc, prepend_padding(sconv, _utoa(sconv, t.hour), PAD_ZEROES, 2));
        uart_print(&bc, ":");
        uart_print(&bc, prepend_padding(sconv, _utoa(sconv, t.min), PAD_ZEROES, 2));
        uart_print(&bc, ":");
        uart_print(&bc, prepend_padding(sconv, _utoa(sconv, t.sec), PAD_ZEROES, 2));

        // there is a compile time option in the library to include unixtime support
#ifdef CONFIG_UNIXTIME
        uart_print(&bc, "  ");
        uart_print(&bc, _utoa(sconv, t.unixtime));
#endif
        uart_print(&bc, "\r\n");
    } else if (strstr(input, "tw")) {
        t.sec = 0;
        t.min = COMPILE_MIN;
        t.hour = COMPILE_HOUR;
        t.wday = COMPILE_DOW;
        t.mday = COMPILE_DAY;
        t.mon = COMPILE_MON;
        t.year = COMPILE_YEAR;
        t.yday = 0;
        t.isdst = 0;
        t.year_s = 0;
#ifdef CONFIG_UNIXTIME
        t.unixtime = 0;
#endif
        DS3231_set(I2C_BASE_ADDR, t);
#endif
    } else {
        //uart_tx_str("\r\n", 2);
    }
}
