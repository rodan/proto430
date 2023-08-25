#ifndef __ADC_H__
#define __ADC_H__

#define       ADC_1_CELL  0x1
#define       ADC_2_CELL  0x2
#define           ADC_ID  0x4

#define      ADC_EV_NULL  0
#define  ADC_EV_CONV_RDY  0x1

struct adc_t {
    uint16_t adc_ccp;  // which pin is used during current conversion
    uint16_t adc_mem;
};


#ifdef __cplusplus
extern "C" {
#endif

void vref_init(void);
void vref_halt(void);
uint8_t adc_init(const uint8_t pins);
void adc_halt(void);
uint16_t adc_get_event(void);
void adc_rst_event(void);
struct adc_t * adc_get_p(void);

#ifdef __cplusplus
}
#endif

#endif
