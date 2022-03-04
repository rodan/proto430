#ifndef __JIG_H__
#define __JIG_H__

// what kind of output to provide during packet parsing
#define          JIG_OUTPUT_NONE  0x0   // silence during parsing
#define            JIG_OUTPUT_MR  0x1   // memory-dump
#define            JIG_OUTPUT_HR  0x4   // human readable
#define           JIG_OUTPUT_HRD  0x8   // human readable, more verbose

#define JIG_ID_SIZE 17
struct jig_t {
    uint8_t type;
    char sn[6];                 // Year Week Count
    uint16_t adc_v1;
    uint16_t adc_v2;
    uint16_t adc_id;
    uint8_t ver_major;
    uint8_t ver_minor;
    uint16_t build;
} __attribute__ ((packed));

void jig_set_output(const uint8_t type);
uint8_t jig_get_output(void);
struct jig_t *jig_get_p(void);

#endif
