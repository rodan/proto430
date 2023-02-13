
#include <stdio.h>
#include <string.h>
#include "glue.h"
#include "version.h"
#include "timer_a2.h"
#include "eeram_48l_extra.h"
#include "vfd_extra.h"
#include "rtca_now.h"
#include "jig.h"
#include "ui.h"

#define TEST_CYPRESS_FM24

extern uart_descriptor bc;
uint8_t brightness = 1;
uint8_t jig_data[23];

#ifdef CONFIG_DS3231
    struct ts t;
#endif

static const char menu_str[]="\
 available commands:\r\n\r\n\
\033[33;1m?\033[0m  - show menu\r\n\
\033[33;1m5v on/off\033[0m  - control 5v rail\r\n\
\033[33;1mtr/tw\033[0m  - test rtc\r\n\
\033[33;1mtj\033[0m     - test jig\r\n\
\033[33;1msr/sw\033[0m  - test eeram\r\n";

//uint8_t refresh_timer;

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

#ifdef TEST_CYPRESS_FM24
void print_buf_fram(const uint32_t address, const uint32_t size)
{
    uint32_t bytes_remaining = size;
    uint32_t bytes_to_be_printed, bytes_printed = 0;
    char itoa_buf[CONV_BASE_10_BUF_SZ];
    uint16_t i;
    uint8_t *read_ptr = NULL;
    uint8_t row[16];

    while (bytes_remaining > 0) {
    
        if (bytes_remaining > 16) {
            bytes_to_be_printed = 16;
        } else {
            bytes_to_be_printed = bytes_remaining;
        }

        uart_print(&bc, _utoh32(itoa_buf, bytes_printed));
        uart_print(&bc, ": ");

        memset(row, 0xee, 16);
        if ( FM24_read(I2C_BASE_ADDR, FM24_SLAVE_ADDR, row, address + bytes_printed, bytes_to_be_printed) != EXIT_SUCCESS) {
            uart_print(&bc, "transfer err\r\n");
            return;
        }

        read_ptr = &row[0];

        for (i = 0; i < bytes_to_be_printed; i++) {
            uart_print(&bc, _utoh8(itoa_buf, *read_ptr++));
            if (i & 0x1) {
                uart_print(&bc, " ");
            }
        }

        //uart_print(&bc, " ");
        //read_ptr = &row[0];
        //for (i = 0; i < bytes_to_be_printed; i++) {
        //    uart_tx_str(&bc, (char *)read_ptr++, 1);
        //}

        uart_print(&bc, "\r\n");
        bytes_printed += bytes_to_be_printed;
        bytes_remaining -= bytes_to_be_printed;
    }
}
#endif

void ui_vfd_refresh(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];
    //uint16_t rnd;
    uint8_t data[4];

    struct meas_ec *p;

    p = (struct meas_ec *) jig_data;

    //rnd = 146 - (rand() % 20);

    //if (!refresh_timer) {
    //vfd_cmd_clear(&vfdd);
    //    refresh_timer = 5;
    //}

    data[0] = 0x1f;
    data[1] = 0x58;
    data[2] = brightness;
    vfd_tx_str(&vfdd, (char *)data, 3);

    //vfd_set_font_style(&vfdd, 1, 1);

    // set prompt to first pixel
    data[0] = 0x1f;
    data[1] = '$';
    vfd_tx_str(&vfdd, (char *)data, 2);
    vfd_cmd_xy(&vfdd, 0, 0);

    // set big font
    vfd_set_font_size(&vfdd, 2, 2, 1);
    vfd_print(&vfdd, " ");

    if (p->ec > 999999999) {
        vfd_print(&vfdd, prepend_padding(sconv, _utoa(sconv, (p->ec + 5000000) / 1000000000), PAD_SPACES, 3));
        vfd_print(&vfdd, ".");
        vfd_print(&vfdd, prepend_padding(sconv, _utoa(sconv, (p->ec + 5000000) % 1000000000 / 10000000), PAD_ZEROES, 2));

        // set prompt to meas unit
        data[0] = 0x1f;
        data[1] = '$';
        vfd_tx_str(&vfdd, (char *)data, 2);
        vfd_cmd_xy(&vfdd, 102, 0);
        vfd_print(&vfdd, " S");

    } else if (p->ec > 999999) {
        vfd_print(&vfdd, prepend_padding(sconv, _utoa(sconv, (p->ec + 5000) / 1000000), PAD_SPACES, 3));
        vfd_print(&vfdd, ".");
        vfd_print(&vfdd, prepend_padding(sconv, _utoa(sconv, (((p->ec + 5000) % 1000000) / 10000)), PAD_ZEROES, 2));

        // set prompt to meas unit
        data[0] = 0x1f;
        data[1] = '$';
        vfd_tx_str(&vfdd, (char *)data, 2);
        vfd_cmd_xy(&vfdd, 102, 0);

        vfd_print(&vfdd, "mS");
    } else if (p->ec > 999) {
        vfd_print(&vfdd, prepend_padding(sconv, _utoa(sconv, (p->ec + 5) / 1000), PAD_SPACES, 3));
        vfd_print(&vfdd, ".");
        vfd_print(&vfdd, prepend_padding(sconv, _utoa(sconv, ((p->ec + 5) % 1000) / 10), PAD_ZEROES, 2));

        // set prompt to meas unit
        data[0] = 0x1f;
        data[1] = '$';
        vfd_tx_str(&vfdd, (char *)data, 2);
        vfd_cmd_xy(&vfdd, 102, 0);

        vfd_tx(&vfdd, 0xe6);
        vfd_print(&vfdd, "S");
    } else {
        vfd_print(&vfdd, prepend_padding(sconv, _utoa(sconv, p->ec), PAD_SPACES, 6));

        // set prompt to meas unit
        data[0] = 0x1f;
        data[1] = '$';
        vfd_tx_str(&vfdd, (char *)data, 2);
        vfd_cmd_xy(&vfdd, 102, 0);

        vfd_print(&vfdd, "nS");
    }

    // set small font
    vfd_set_font_size(&vfdd, 1, 1, 1);

    // automatic or manual ranging
    data[0] = 0x1f;
    data[1] = '$';
    vfd_tx_str(&vfdd, (char *)data, 2);
    vfd_cmd_xy(&vfdd, 132, 0);

    //vfd_invert_on(&vfdd);

    if (p->range & 0x1) {
        vfd_print(&vfdd, "m");
    } else {
        vfd_print(&vfdd, "a");
    }

    data[0] = 0x1f;
    data[1] = '$';
    vfd_tx_str(&vfdd, (char *)data, 2);
    vfd_cmd_xy(&vfdd, 132, 8);

    vfd_print(&vfdd, _utoa(sconv, p->range >> 1));
    //vfd_invert_off(&vfdd);

    // line 3
    // set prompt to first pixel, last line
    data[0] = 0x1f;
    data[1] = '$';
    vfd_tx_str(&vfdd, (char *)data, 2);
    vfd_cmd_xy(&vfdd, 0, 24);

    vfd_print(&vfdd, " ");

    // temperature value

    if (p->temp < -10) {
        vfd_print(&vfdd, "  t Ovf");
    } else if (p->temp < 0) {
        vfd_print(&vfdd, prepend_padding(sconv, _itoa(sconv, (p->temp + 5) / 100), PAD_SPACES, 3));
        vfd_print(&vfdd, ".");
        vfd_print(&vfdd, _utoa(sconv, (((0 - p->temp + 5) / 10) % 10)));
        vfd_tx(&vfdd, 0xf8);
        vfd_print(&vfdd, "C");
    } else if (p->temp < 20000) {
        vfd_print(&vfdd, prepend_padding(sconv, _itoa(sconv, (p->temp + 5) / 100), PAD_SPACES, 3));
        vfd_print(&vfdd, ".");
        vfd_print(&vfdd, _utoa(sconv, (((p->temp + 5) / 10) % 10)));
        vfd_tx(&vfdd, 0xf8);
        vfd_print(&vfdd, "C");
    } else {
        vfd_print(&vfdd, "  t Ovf");
    }


    // set prompt to data area
    data[0] = 0x1f;
    data[1] = '$';
    vfd_tx_str(&vfdd, (char *)data, 2);
    vfd_cmd_xy(&vfdd, 84, 24);

#ifdef CONFIG_DS3231
    vfd_print(&vfdd, prepend_padding(sconv, _utoa(sconv, t.hour), PAD_ZEROES, 2));
    vfd_print(&vfdd, ":");
    vfd_print(&vfdd, prepend_padding(sconv, _utoa(sconv, t.min), PAD_ZEROES, 2));
    vfd_print(&vfdd, ":");
    vfd_print(&vfdd, prepend_padding(sconv, _utoa(sconv, t.sec), PAD_ZEROES, 2));
#endif

    timer_a2_set_trigger_slot(SCHEDULE_RTC_REFRESH, systime()+20, TIMER_A2_EVENT_ENABLE);
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
        //timer_a2_set_trigger_slot(SCHEDULE_VFD_REFRESH, systime()+100, TIMER_A2_EVENT_ENABLE);
    } else if (strstr(input, "clr")) {
        timer_a2_set_trigger_slot(SCHEDULE_VFD_REFRESH, 0, TIMER_A2_EVENT_DISABLE);
        vfd_cmd_clear(&vfdd);
    } else if (strstr(input, "sr")) {
        EERAM_48L_read_streg(&spid_eeram, &val);
    } else if (strstr(input, "sw")) {
        EERAM_48L_write_streg(&spid_eeram, EERAM_48L512_SR_ASE );
    } else if (strstr(input, "tj")) {
        jig_7000_read(0x0, jig_data, 23);
#ifdef TEST_CYPRESS_FM24
    } else if (f == 'h') {
        //print_buf_fram(0x0, 0x4);
        //print_buf_fram(0x0, 0xaa10);
        print_buf_fram(0x1835, 0x80);
        //print_buf_fram(0x0, 0xffff);
#endif
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
