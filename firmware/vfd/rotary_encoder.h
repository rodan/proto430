#ifndef __ROTARY_ENCODER_H_
#define __ROTARY_ENCODER_H_

/////////////////////////////////////////////////////////////////////
// constants
/////////////////////////////////////////////////////////////////////

// #define if the rotary encoder used ends up at a high idle on both channels
// after entering every detent (specified number of detents == number of pulses)
//#define         ROT_ENC_FULL_CYCLE

// Full cycle encoders tested:
// Bourns PEC11R-4215F-S0024    - 24 detents and 24 pulses per 360deg turn
//                              - weak detent spring
//                              - 0/5 would never buy again, very noisy

// Half cycle encoders tested:
// Bourns PEC11L-4225F-S0015    - 30 detents and 15 pulses per 360deg turn
//                              - strong detent spring, low profile
//                              - 4/5 me like, great quality

#ifdef ROT_ENC_FULL_CYCLE
    // the sample encoder was extremely noisy, so max out the debouncing period
    #define       ROT_DEBOUNCE_PERIOD  0xff
#else
    #define       ROT_DEBOUNCE_PERIOD  0x7f
#endif

// ROT_DEBOUNCE_PERIOD conversion table:
// 0x1f - 1ms
// 0x3f ~ 2ms
// 0x5f ~ 3ms
// 0x7f ~ 4ms
// 0x9f ~ 5ms
// 0xbf ~ 6ms
// 0xdf ~ 7ms
// 0xff ~ 8ms

#define ROT_ENC_CHA_IN      ( P8IN & BIT2 )
#define ROT_ENC_CHB_IN      ( P8IN & BIT1 )

typedef struct
{
    int8_t change_A;
    int8_t debounced_A;
    int8_t debounced_B;
    uint8_t debouncing_A;
    uint8_t counter;
} encoder_t;

extern encoder_t enc;

//extern uint8_t rot_enc_changed;
//extern uint8_t rot_enc_pressed;
//extern int16_t rot_enc_value;

void rot_enc_init(void);
void rot_enc_increment(void);
void rot_enc_decrement(void);
void rot_enc_manager(void);

#endif
