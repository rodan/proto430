#ifndef __EERAM_48L_EXTRA_H__
#define __EERAM_48L_EXTRA_H__

extern spi_descriptor spid_eeram;

#ifdef __cplusplus
extern "C" {
#endif

void EERAM_48L_CS_high(void);
void EERAM_48L_CS_low(void);
void EERAM_48L_HOLD_assert(void);
void EERAM_48L_HOLD_deassert(void);
void EERAM_48L_init(void);

#ifdef __cplusplus
}
#endif

#endif
