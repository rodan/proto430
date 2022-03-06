#ifndef __IR_REMOTE_EXTRA_H__
#define __IR_REMOTE_EXTRA_H__


// port/pin declaration 
#define   IR_IES  P6IES
#define    IR_IE  P6IE
#define   IR_IFG  P6IFG
#define   IR_DIR  P6DIR
#define    IR_IN  P6IN
#define   IR_PIN  BIT3

// the IR signal is about 25ms to stop the decoding timer after about 30ms
// (each tick is 250us)
#define  IR_TIMEOUT_TICKS  180

// flags used by the ir_remote_init() function
#define        IR_DEFAULT  0 ///< function will just do the pin setup
#define         IR_RST_SM  1 ///< also reset the decoding state machine

#ifdef __cplusplus
extern "C" {
#endif


void ir_remote_init(uint8_t flag);
uint8_t ir_remote_in(void);
void ir_remote_mng(void);

#ifdef __cplusplus
}
#endif

#endif
