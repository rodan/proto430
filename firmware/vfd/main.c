
/*
    reference MSP430 library for an HI6000 EDGE probe

    tweak the baud rate in config.h
*/

#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "proj.h"
#include "driverlib.h"
#include "glue.h"
#include "ui.h"
#include "timer_a1.h"
#include "timer_a2.h"
#include "timer_a3.h"
#include "adc.h"
#include "list_helper.h"
#include "version.h"
#include "jig.h"
#include "sig.h"
#include "rotary_encoder.h"
#include "eeram_48l_extra.h"
#include "vfd_extra.h"
#include "ir_remote_extra.h"

volatile uint8_t port3_last_event;
volatile uint8_t port5_last_event;
uint32_t button_down_start;

uint32_t vbat_cell_1;
uint32_t vbat_cell_2;
uint32_t vbat_cell_t;

uart_descriptor bc; // backchannel uart interface
extern struct ts t;

#define ADC_REFRESH_INTERVAL 3600UL // time in seconds between consecutive measurements of the battery voltage

void main_init(void)
{
    P3DIR = 0x3d;

    // activate pullup
    P3OUT = 0xc0;
    P3REN = 0xc0;

    // IRQ triggers on rising edge
    P3IES &= ~BIT1;

    // P44 is display BSY
    P4DIR = 0xef;

    // P55 and P56 are buttons
    // P57 is rot enc sw
    P5DIR = 0x1f;

    // activate pullup
    P5OUT = 0x60;
    P5REN = 0x60;

    // IRQ triggers on the falling edge
    P5IES = 0xe0;

    // P63 is the IR sensor
    P6DIR = 0xf7;

    // P81 and P82 are rot encoder signals
    P8DIR = 0xf9;

    rail_3v3_on;
}

void i2c_init(void)
{
    i2c_pin_init();

#if I2C_USE_DEV > 3
    // enhanced USCI capable microcontroller
    EUSCI_B_I2C_initMasterParam param = {0};

    param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = SMCLK_FREQ;
    param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_400KBPS;
    param.byteCounterThreshold = 0;
    param.autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP;
    EUSCI_B_I2C_initMaster(I2C_BASE_ADDR, &param);
#elif I2C_USE_DEV < 4
    // USCI capable microcontroller
    USCI_B_I2C_initMasterParam param = {0};

    param.selectClockSource = USCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = SMCLK_FREQ;
    param.dataRate = USCI_B_I2C_SET_DATA_RATE_400KBPS;
    USCI_B_I2C_initMaster(I2C_BASE_ADDR, &param);
#endif

    i2c_irq_init(I2C_BASE_ADDR);
}

void spi_init(void)
{

    spi_pin_init();

#if defined __MSP430_HAS_EUSCI_Bx__
    EUSCI_B_SPI_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK;
    param.clockSourceFrequency = SMCLK_FREQ;
    param.desiredSpiClock = 1000000;
    param.msbFirst= EUSCI_B_SPI_MSB_FIRST;
    param.clockPhase= EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    param.spiMode = EUSCI_B_SPI_3PIN;
    EUSCI_B_SPI_initMaster(SPI_BASE_ADDR, &param);
    EUSCI_B_SPI_enable(SPI_BASE_ADDR);

#elif defined __MSP430_HAS_USCI_Bx__
    USCI_B_SPI_initMasterParam param = {0};
    param.selectClockSource = USCI_B_SPI_CLOCKSOURCE_SMCLK;
    param.clockSourceFrequency = SMCLK_FREQ;
    param.desiredSpiClock = 1000000;
    param.msbFirst= USCI_B_SPI_MSB_FIRST;
    param.clockPhase= USCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = USCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    USCI_B_SPI_initMaster(SPI_BASE_ADDR, &param);
    USCI_B_SPI_enable(SPI_BASE_ADDR);
#endif
}

void spi_pause(void)
{
    // zero out BSY signal
    //P3IE &= ~BIT7;
    //P3OUT &= ~BIT7;

    // zero out SPI pins
    //P5SEL0 &= ~(BIT0 | BIT1 | BIT2);
    //P5SEL1 &= ~(BIT0 | BIT1 | BIT2);
    //P5DIR &= ~(BIT0 | BIT1 | BIT2 | BIT3);
    //P5OUT &= ~(BIT0 | BIT1 | BIT2);
    //P5REN |= BIT0 | BIT1 | BIT2;
}

void spi_resume(void)
{
    // restore BSY signal
    //P3IFG &= ~BIT7;
    //P3OUT |= BIT7;
    //P3IE |= BIT7;

    // restore SPI pins
    //P5REN &= (BIT0 | BIT1 | BIT2);
    //spi_pin_init();
    //P5DIR |= BIT3;
}

static void button_31_irq(uint32_t msg)
{
    if (P3IN & BIT1) {
        timer_a2_set_trigger_slot(SCHEDULE_PB_31_OFF, systime() + 300, TIMER_A2_EVENT_ENABLE);
        timer_a2_set_trigger_slot(SCHEDULE_POWER_SAVING, systime() + POWER_SAVING_DELAY,
                                  TIMER_A2_EVENT_ENABLE);
#ifdef P31_ADC
        // Reset IRQ flags
        P3IFG &= ~BIT1;
        // disable button interrupt
        P3IE &= ~BIT1;

        // start vref
        vref_init();
        // set pin as analog input
        //adc_init(ADC_1_CELL);
        P3SEL1 |= BIT1;
        P3SEL0 |= BIT1;
        // trigger a conversion in a few ms
        timer_a2_set_trigger_slot(SCHEDULE_ADC_1CELL, systime() + 3, TIMER_A2_EVENT_ENABLE);
#else
        // IRQ triggers on falling edge
        P3IES |= BIT1;
#endif
    } else {
        timer_a2_set_trigger_slot(SCHEDULE_PB_31_OFF, 0, TIMER_A2_EVENT_DISABLE);
        // IRQ triggers on rising edge
        P3IES &= ~BIT1;
    }
}

static void jig_ready_handler(uint32_t msg)
{
    jig_7000_read(0x0, jig_data, 23);
    
    if (P1OUT & BIT2) {
        ui_vfd_refresh();
    }
}

static void button_55_irq(uint32_t msg)
{
    if (P5IN & BIT5) {
        timer_a2_set_trigger_slot(SCHEDULE_PB_55, 0, TIMER_A2_EVENT_DISABLE);
    } else {
        timer_a2_set_trigger_slot(SCHEDULE_PB_55, systime() + 100, TIMER_A2_EVENT_ENABLE);
    }
}

static void button_55_long_press_irq(uint32_t msg)
{
    uart_print(&bc, "PB55 long\r\n");
}

static void button_56_irq(uint32_t msg)
{
    if (P5IN & BIT6) {
        sig0_off;
        timer_a2_set_trigger_slot(SCHEDULE_PB_56, 0, TIMER_A2_EVENT_DISABLE);
    } else {
        timer_a2_set_trigger_slot(SCHEDULE_PB_56, systime() + 100, TIMER_A2_EVENT_ENABLE);
    }
}

static void button_56_long_press_irq(uint32_t msg)
{
    uart_print(&bc, "PB56 long\r\n");
}

static void button_57_irq(uint32_t msg)
{
    if (P5IN & BIT7) {
        sig0_off;
        timer_a2_set_trigger_slot(SCHEDULE_PB_57, 0, TIMER_A2_EVENT_DISABLE);
    } else {
        timer_a2_set_trigger_slot(SCHEDULE_PB_57, systime() + 100, TIMER_A2_EVENT_ENABLE);
    }
}

static void button_57_long_press_irq(uint32_t msg)
{
    uart_print(&bc, "PB57 long\r\n");

    rail_5v_on;
    //timer_a2_set_trigger_slot(SCHEDULE_VFD_REFRESH, systime()+100, TIMER_A2_EVENT_ENABLE);
    //timer_a2_set_trigger_slot(SCHEDULE_RTC_REFRESH, systime()+20, TIMER_A2_EVENT_ENABLE);
}

/*
static void led_on_irq(uint32_t msg)
{
    sig4_on;
    timer_a2_set_trigger_slot(SCHEDULE_LED_OFF, systime() + 100, TIMER_A2_EVENT_ENABLE);
}

static void led_off_irq(uint32_t msg)
{
    sig4_off;
    timer_a2_set_trigger_slot(SCHEDULE_LED_ON, systime() + 100, TIMER_A2_EVENT_ENABLE);
}
*/

static void poweroff_irq(uint32_t msg)
{
    halt();
}

static void scheduler_irq(uint32_t msg)
{
    timer_a2_scheduler_handler();
}

void halt(void)
{
    rail_5v_off;
    rail_3v3_off;
    sig0_on;
    sig1_off;
    sig2_off;
    sig3_off;
    sig4_off;
}

/*
static void vfd_refresh(uint32_t msg)
{
    ui_vfd_refresh();
    timer_a2_set_trigger_slot(SCHEDULE_VFD_REFRESH, systime()+100, TIMER_A2_EVENT_ENABLE);
}
*/

static void rtc_refresh(uint32_t msg)
{
    DS3231_get(I2C_BASE_ADDR, &t);
    timer_a2_set_trigger_slot(SCHEDULE_RTC_REFRESH, systime()+100, TIMER_A2_EVENT_ENABLE);
}

static void adc_rdy_irq(uint32_t msg)
{
    struct jig_t *jig;
    struct adc_t *adc;

    adc = adc_get_p();

    switch (adc->adc_ccp) {
#ifdef P31_ADC
    case ADC_1_CELL:
        // set pin as digital input with edge triggering
        P3SEL1 &= ~BIT1;
        P3SEL0 &= ~BIT1;
        P3DIR &= ~BIT1;
        if (P3IN & BIT1) {
            // user keeps pushing button
            // IRQ triggers on falling edge
            P3IES |= BIT1;
        } else {
            timer_a2_set_trigger_slot(SCHEDULE_PB_31_OFF, 0, TIMER_A2_EVENT_DISABLE);
            // IRQ triggers on rising edge
            P3IES &= ~BIT1;
        }

        vbat_cell_1 = (uint32_t) adc->adc_mem *(uint32_t) 100 / (uint32_t) 902;      // 4096/(2.5 * 4.178 / 2.30)

        // Reset IRQ flags
        P3IFG &= ~BIT1;
        // Enable button interrupt
        P3IE |= BIT1;
        break;
 #endif
    case ADC_2_CELL:
        vbat_cell_t = (uint32_t) adc->adc_mem *(uint32_t) 100 / (uint32_t) 478;      // 4096/(2.5*8.368/2.442)
        //adc_2cell_check_off;
        break;
    case ADC_ID:
        break;
    default:
        break;
    }

    if (vbat_cell_t && vbat_cell_1) {
        vbat_cell_2 = vbat_cell_t - vbat_cell_1;
    }

    jig = jig_get_p();
    jig->adc_v1 = vbat_cell_1;
#ifdef P31_ADC
    jig->adc_v2 = vbat_cell_2;
#else
    jig->adc_v2 = vbat_cell_t;
#endif

    vref_halt();
    adc_halt();
}

#ifdef P31_ADC
static void adc_1cell_conv_start(uint32_t msg)
{
    struct adc_t *adc;

    if (P3IN & BIT1) {
        vref_init();
        adc_halt();
        ADC12MCTL0 = ADC12VRSEL_1 | ADC12INCH_13;
        ADC12CTL0 |= ADC12ON;
        ADC12CTL0 |= ADC12ENC | ADC12SC;
        adc = adc_get_p();
        adc->adc_ccp = ADC_1_CELL;
        //timer_a2_set_trigger_slot(SCHEDULE_ADC_1CELL, systime() + (ADC_REFRESH_INTERVAL * 100), TIMER_A2_EVENT_ENABLE);
    }
}
#endif

static void adc_2cell_conv_start(uint32_t msg)
{
    struct adc_t *adc;

    adc = adc_get_p();
    vref_init();
    adc_halt();
    ADC12MCTL0 = ADC12VRSEL_1 | ADC12INCH_14;
    ADC12CTL0 |= ADC12ON;
    ADC12CTL0 |= ADC12ENC | ADC12SC;
    adc->adc_ccp = ADC_2_CELL;
    timer_a2_set_trigger_slot(SCHEDULE_ADC_2CELL, systime() + (ADC_REFRESH_INTERVAL * 100), TIMER_A2_EVENT_ENABLE);
}

static void uart_bcl_rx_irq(uint32_t msg)
{
    parse_user_input();
    uart_set_eol(&bc);
    timer_a2_set_trigger_slot(SCHEDULE_POWER_SAVING, systime() + POWER_SAVING_DELAY,
                              TIMER_A2_EVENT_ENABLE);
}

#ifdef CONFIG_IR_RECEIVER
static void ir_remote_irq(uint32_t msg)
{
    ir_remote_mng();
}
#endif

void check_events(void)
{
    uint32_t msg = SYS_MSG_NULL;
    uint16_t ev;

    sig2_on;
    sig3_on;

    // uart RX
    if (uart_get_event(&bc) & UART_EV_RX) {
        msg |= SYS_MSG_UART_BCL_RX;
        uart_rst_event(&bc);
    }
    #if 0
    // vfd
    if (VFD_get_event(NULL) & VFD_EV_RDY) {
        msg |= SYS_MSG_VFD_TX_RDY;
        VFD_rst_event(NULL);
    }
    #endif
    #if 0
    // timer_a0
    ev = timer_a0_get_event();
    if (ev) {
        if (ev & TIMER_A0_EVENT_CCR1) {
            msg |= SYS_MSG_TIMERA0_CCR1;
        } else if (ev & TIMER_A0_EVENT_CCR2) {
            msg |= SYS_MSG_TIMERA0_CCR2;
        }
        timer_a0_rst_event();
    }
    #endif
    // timer_a1
#ifdef CONFIG_IR_RECEIVER
    ev = timer_a1_get_event();
    if (ev) {
        if (ev & TIMER_A1_EVENT_CCR0) {
            msg |= SYS_MSG_TIMERA1_CCR0;
        }
        timer_a1_rst_event();
    }
#endif
    // timer_a2
    ev = timer_a2_get_event();
    if (ev) {
        if (ev & TIMER_A2_EVENT_CCR1) {
            msg |= SYS_MSG_TIMERA2_CCR1;
        }
        timer_a2_rst_event();
    }

    // timer_a2-based scheduler
    ev = timer_a2_get_event_schedule();
    if (ev) {
        if ((ev & (1 << SCHEDULE_POWER_SAVING)) || (ev & (1 << SCHEDULE_PB_31_OFF))) {
            msg |= SYS_MSG_SCH_POWEROFF;
        }
        if (ev & (1 << SCHEDULE_ADC_2CELL)) {
            msg |= SYS_MSG_SCH_CONV_2CELL;
        }
        if (ev & (1 << SCHEDULE_ADC_1CELL)) {
            msg |= SYS_MSG_SCH_CONV_1CELL;
        }
        if (ev & (1 << SCHEDULE_PB_55)) {
            msg |= SYS_MSG_P55_TMOUT_INT;
        }
        if (ev & (1 << SCHEDULE_PB_56)) {
            msg |= SYS_MSG_P56_TMOUT_INT;
        }
        if (ev & (1 << SCHEDULE_PB_57)) {
            msg |= SYS_MSG_P57_TMOUT_INT;
        }
        if (ev & (1 << SCHEDULE_VFD_REFRESH)) {
            msg |= SYS_MSG_VFD_REFRESH;
        }
        if (ev & (1 << SCHEDULE_RTC_REFRESH)) {
            msg |= SYS_MSG_RTC_REFRESH;
        }

        timer_a2_rst_event_schedule();
    }

    // adc conversion
    ev = adc_get_event();
    if (ev) {
        msg |= SYS_MSG_ADC_CONV_RDY;
        adc_rst_event();
    }
    // push button P3.1, P3.3 int
    if (port3_last_event) {
        if (port3_last_event & BIT1) {
            msg |= SYS_MSG_P31_INT;
            port3_last_event ^= BIT1;
        }
        if (port3_last_event & BIT7) {
            msg |= SYS_MSG_JIG_7000_RDY;
            port3_last_event ^= BIT7;
        }
        port3_last_event = 0;
    }
    // push button P5.x
    if (port5_last_event) {
        if (port5_last_event & BIT5) {
            msg |= SYS_MSG_P55_INT;
            port5_last_event ^= BIT5;
        }
        if (port5_last_event & BIT6) {
            msg |= SYS_MSG_P56_INT;
            port5_last_event ^= BIT6;
        }
        if (port5_last_event & BIT7) {
            msg |= SYS_MSG_P57_INT;
            port5_last_event ^= BIT7;
        }
    }

    sig3_off;
    eh_exec(msg);
    sig2_off;
}

int main(void)
{
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    msp430_hal_init(HAL_GPIO_DIR_OUTPUT | HAL_GPIO_OUT_LOW);
#ifdef USE_SIG
    sig0_on;
#endif

    main_init();
    EERAM_48L_CS_high();
    VFD_CS_high();
    jig_7000_CS_high();

    clock_pin_init();
    clock_init();

    //timer_a0_init();            //
    timer_a2_init();    // scheduler, systime()
    timer_a3_init();    // rotary decoder

    bc.baseAddress = EUSCI_A0_BASE;
    bc.baudrate = BAUDRATE_57600;

    uart_pin_init(&bc);
    uart_init(&bc);
#if defined UART_RX_USES_RINGBUF
    uart_set_rx_irq_handler(&bc, uart_rx_ringbuf_handler);
#else
    uart_set_rx_irq_handler(&bc, uart_rx_simple_handler);
#endif

    adc_init(ADC_2_CELL);

    rot_enc_init();

#ifdef CONFIG_IR_RECEIVER
    ir_remote_init(IR_RST_SM);
#endif

    i2c_init();
    spi_init();

    EERAM_48L_init();
    VFD_init();
    jig_7000_init();
    spi_pause();

    sig0_off;
    sig1_off;
    sig2_off;
    sig3_off;

    eh_init();
    eh_register(&uart_bcl_rx_irq, SYS_MSG_UART_BCL_RX);
    eh_register(&button_31_irq, SYS_MSG_P31_INT);
    eh_register(&button_55_irq, SYS_MSG_P55_INT);
    eh_register(&button_56_irq, SYS_MSG_P56_INT);
    eh_register(&button_57_irq, SYS_MSG_P57_INT);

    eh_register(&poweroff_irq, SYS_MSG_SCH_POWEROFF);
    eh_register(&button_55_long_press_irq, SYS_MSG_P55_TMOUT_INT);
    eh_register(&button_56_long_press_irq, SYS_MSG_P56_TMOUT_INT);
    eh_register(&button_57_long_press_irq, SYS_MSG_P57_TMOUT_INT);

#ifdef CONFIG_IR_RECEIVER
    eh_register(&ir_remote_irq, SYS_MSG_TIMERA1_CCR0);
#endif
#ifdef P31_ADC
    eh_register(&adc_1cell_conv_start, SYS_MSG_SCH_CONV_1CELL);
#endif
    eh_register(&adc_2cell_conv_start, SYS_MSG_SCH_CONV_2CELL);
    eh_register(&adc_rdy_irq, SYS_MSG_ADC_CONV_RDY);

    eh_register(&scheduler_irq, SYS_MSG_TIMERA2_CCR1);
    //eh_register(&vfd_refresh, SYS_MSG_VFD_REFRESH);
    eh_register(&rtc_refresh, SYS_MSG_RTC_REFRESH);
    
    eh_register(&jig_ready_handler, SYS_MSG_JIG_7000_RDY);

    timer_a2_set_trigger_slot(SCHEDULE_POWER_SAVING, POWER_SAVING_DELAY, TIMER_A2_EVENT_ENABLE);
//    timer_a2_set_trigger_slot(SCHEDULE_LED_ON, 200, TIMER_A2_EVENT_ENABLE);

    //display_menu();

    // Reset IRQ flags
    P3IFG = 0;
    // Enable button interrupt
    P3IE |= BIT1;

    // Reset IRQ flags
    P5IFG &= ~(BIT5 | BIT6 | BIT7);
    // Enable button interrupt
    P5IE |= BIT5 | BIT6 | BIT7;

    // fake button interrupt so we read the battery voltage
    button_31_irq(0);

    adc_2cell_check_on;
    vref_init();

#ifdef P31_ADC
    timer_a2_set_trigger_slot(SCHEDULE_ADC_1CELL, 5, TIMER_A2_EVENT_ENABLE);
#endif
    timer_a2_set_trigger_slot(SCHEDULE_ADC_2CELL, 50, TIMER_A2_EVENT_ENABLE);

    _BIS_SR(GIE);

    while (1) {
        // sleep
#ifdef LED_SYSTEM_STATES
        sig4_off;
#endif
        _BIS_SR(LPM3_bits + GIE);
#ifdef LED_SYSTEM_STATES
        sig4_on;
#endif
        __no_operation();
//#ifdef USE_WATCHDOG
//        WDTCTL = (WDTCTL & 0xff) | WDTPW | WDTCNTCL;
//#endif
        check_events();
        check_events();
        check_events();
    }
}

// Port 3 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT3_VECTOR
__interrupt void port3_isr_handler(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT3_VECTOR))) port3_isr_handler(void)
#else
#error Compiler not supported!
#endif
{
    switch (P3IV) {
    case P3IV__P3IFG1:
        port3_last_event |= BIT1;
        // listen for opposite edge
        P3IES ^= BIT1;
        //P3IFG &= ~TRIG1;
        _BIC_SR_IRQ(LPM3_bits);
        break;
    case P3IV__P3IFG7:
        port3_last_event |= BIT7;
        _BIC_SR_IRQ(LPM3_bits);
        break;
    }
    P3IFG = 0;
}

// Port 5 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT5_VECTOR
__interrupt void port5_isr_handler(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT5_VECTOR))) port5_isr_handler(void)
#else
#error Compiler not supported!
#endif
{
    switch (P5IV) {
    case P5IV__P5IFG5:
        port5_last_event |= BIT5;
        // listen for opposite edge
        P5IES ^= BIT5;
        _BIC_SR_IRQ(LPM3_bits);
        break;
    case P5IV__P5IFG6:
        port5_last_event |= BIT6;
        // listen for opposite edge
        P5IES ^= BIT6;
        _BIC_SR_IRQ(LPM3_bits);
        break;
    case P5IV__P5IFG7:
        port5_last_event |= BIT7;
        // listen for opposite edge
        P5IES ^= BIT7;
        _BIC_SR_IRQ(LPM3_bits);
        break;
    }
    P5IFG = 0;
}
