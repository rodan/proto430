
#include <stdio.h>
#include <string.h>
#include "glue.h"
#include "version.h"
#include "timer_a2.h"
#include "jig.h"
#include "ui.h"

#define TEST_CYPRESS_FM24

extern uart_descriptor bc;
uint8_t brightness = 1;
uint8_t jig_data[23];

static const char menu_str[]="\
 available commands:\r\n\r\n\
\033[33;1m?\033[0m  - show menu\r\n\
\033[33;1m5v on/off\033[0m  - control 5v rail\r\n";

static const char menu_eeprom[]="\
 \033[33;1mr\033[0m - eeprom read 0x0000 - 0xffff\r\n";


#ifdef TEST_CYPRESS_FM24
#if 0
static const char menu_CYPRESS_FM24[]="\
 \033[33;1mt\033[0m - CYP_FM24 memtest\r\n\
 \033[33;1mr\033[0m - CYP_FM24 tiny read test\r\n\
 \033[33;1mw\033[0m - CYP_FM24 tiny write test\r\n\
 \033[33;1mh\033[0m - CYP_FM24 hex dump of FRAM segment\r\n";
#endif
#endif

#ifdef TEST_CYPRESS_FM24
void display_memtest(const uint16_t usci_base_addr, const uint8_t slave_addr, const uint32_t start_addr, const uint32_t stop_addr, FM24_test_t test)
{
    uint32_t el;
    uint32_t rows_tested;
    char itoa_buf[CONV_BASE_10_BUF_SZ];

    uart_print(&bc, " \033[36;1m*\033[0m testing ");
    uart_print(&bc, _utoh32(itoa_buf, start_addr));
    uart_print(&bc, " - ");
    uart_print(&bc, _utoh32(itoa_buf, stop_addr));
    uart_print(&bc, " with pattern #");
    uart_print(&bc, _utoa(itoa_buf, test));
    uart_print(&bc, "    ");

    el = FM24_memtest(usci_base_addr, slave_addr, start_addr, stop_addr, test, &rows_tested);

    uart_print(&bc, _utoa(itoa_buf, rows_tested * 8));
    if (el == 0) { 
        uart_print(&bc, " bytes tested \033[32;1mok\033[0m\r\n");
    } else {
        uart_print(&bc, " bytes tested with \033[31;1m");
        uart_print(&bc, _utoa(itoa_buf, el));
        uart_print(&bc, " errors\033[0m\r\n");
    }
}

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

        // print address column
        uart_print(&bc, _utoh32(itoa_buf, bytes_printed));
        uart_print(&bc, ": ");

        memset(&row, 0, 16);

        if ( FM24_read(I2C_BASE_ADDR, FM24_SLAVE_ADDR, row, address + bytes_printed, bytes_to_be_printed) != EXIT_SUCCESS) {
            uart_print(&bc, " read error\r\n");
            return;
        }
        read_ptr = &row[0];

        // print dump in hex
        for (i = 0; i < bytes_to_be_printed; i++) {
            uart_print(&bc, _utoh8(itoa_buf, *read_ptr++));
            if (i & 0x1) {
                uart_print(&bc, " ");
            }
        }

        #if 0
        uart_print(&bc, " ");

        read_ptr = &row[0];
        for (i = 0; i < bytes_to_be_printed; i++) {
            uart_tx_str(&bc, (char *)read_ptr++, 1);
        }
        #endif

        uart_print(&bc, "\r\n");
        bytes_printed += bytes_to_be_printed;
        bytes_remaining -= bytes_to_be_printed;
    }
}
#endif


//uint8_t refresh_timer;

void display_menu(void)
{
    display_version();
    uart_print(&bc, menu_str);
    uart_print(&bc, menu_eeprom);
}

void display_version(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];

    uart_print(&bc, "i2c demo b");
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

#ifdef TEST_CYPRESS_FM24
    //uint8_t data_r[8]; // test 8 bytes (1 row) at a time
    //uint8_t foo[9]="hello wo";
    //uint8_t bar[9]="33333333";
#endif

    if (f == '?') {
        display_menu();
    } else if (strstr(input, "5v on")) {
        rail_5v_on;
    } else if (strstr(input, "5v off")) {
        rail_5v_off;
#ifdef TEST_CYPRESS_FM24
#if 0
    } else if (f == 'w') {
        //FM24_write(I2C_BASE_ADDR, FM24_SLAVE_ADDR, foo, FM_LA-20, 8);
        FM24_write(I2C_BASE_ADDR, FM24_SLAVE_ADDR, bar, 0, 8);
    } else if (f == 'b') {
        //FM24_write(I2C_BASE_ADDR, FM24_SLAVE_ADDR, bar, 0x90, 8);
    } else if (f == 'r') {
        //FM24_read(I2C_BASE_ADDR, FM24_SLAVE_ADDR, data_r, FM_LA-20, 8);
        FM24_read(I2C_BASE_ADDR, FM24_SLAVE_ADDR, data_r, 0, 8);
    } else if (f == 't') {
        //display_memtest(I2C_BASE_ADDR, 0xffe0, FM_LA, TEST_00);
        //display_memtest(I2C_BASE_ADDR, 0xffe0, FM_LA, TEST_00);
        //display_memtest(I2C_BASE_ADDR, 0x10, 0xb0, TEST_AA);
        //display_memtest(I2C_BASE_ADDR, 0x40, 0x60, TEST_FF);
        //display_memtest(I2C_BASE_ADDR, 0x60, 0x80, TEST_AA);
        //display_memtest(I2C_BASE_ADDR, 0x90, 0x98, TEST_00);

        display_memtest(I2C_BASE_ADDR, FM24_SLAVE_ADDR, 0, FM_LA, TEST_FF);
        display_memtest(I2C_BASE_ADDR, FM24_SLAVE_ADDR, 0, FM_LA, TEST_00);
        display_memtest(I2C_BASE_ADDR, FM24_SLAVE_ADDR, 0, FM_LA, TEST_CNT);
        display_memtest(I2C_BASE_ADDR, FM24_SLAVE_ADDR, 0, FM_LA, TEST_AA);
        uart_print(&bc, " * roll over test\r\n");
        display_memtest(I2C_BASE_ADDR, FM24_SLAVE_ADDR, FM_LA - 3, FM_LA + 5, TEST_CNT);
#endif
    } else if (f == 'r') {
        //print_buf_fram(0, 0x10000);
        print_buf_fram(0, 0x10000);
#endif
    } else {
        //uart_tx_str("\r\n", 2);
    }
}
