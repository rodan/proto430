#ifndef __VFD_EXTRA_H__
#define __VFD_EXTRA_H__

extern spi_descriptor spid_vfd;
extern vfd_descriptor vfdd;


#ifdef __cplusplus
extern "C" {
#endif

void VFD_EXTRA_CS_high(void);
void VFD_EXTRA_CS_low(void);
uint8_t VFD_BSY(void);
void VFD_RST_assert(void);
void VFD_RST_deassert(void);
void VFD_init(void);

#ifdef __cplusplus
}
#endif

#endif
