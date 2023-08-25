#ifndef _CONFIG_H_
#define _CONFIG_H_

#define SMCLK_FREQ_16M
//#define SMCLK_FREQ_16M

#define USE_XT1
// the devboard has a 16M crystal installed
#ifdef SMCLK_FREQ_16M
#define USE_XT2
#endif

#define UART_USES_UCA0
#define UART_RX_USES_RINGBUF
#define UART_RXBUF_SZ 16
#define UART_TX_USES_IRQ
#define UART_TXBUF_SZ 16

#define VFD_TXBUF_SZ 64

#define SPI_USES_UCB1
#define SPI_BASE_ADDR EUSCI_B1_BASE

#define USE_I2C_MASTER
//#define CONFIG_DS3231
#define CONFIG_CYPRESS_FM24
#define CONFIG_FM24CL64B
//#define CONFIG_FM24V10

//#define VFD_USES_SPI
//#define CONFIG_VFD_NORITAKE
//#define CONFIG_EERAM_48LXXX

//#define CONFIG_ROT_ENCODER
//#define CONFIG_IR_RECEIVER

// enable in order to have the following led setup:
// led 2 - on during timer interrupt operation
// led 3 - on during meter uart operation
// led 4 - on when uC is outside of LPM3 (low power mode)
//#define LED_SYSTEM_STATES

#define USE_SIG
//#define CONFIG_DEBUG

// we need a lot of event handlers in this project
#define EH_MAX  24

#endif
