#ifndef __VFD_EXTRA_H__
#define __VFD_EXTRA_H__

extern spi_descriptor spid_vfd;
extern vfd_descriptor vfdd;

#define VFD_EV_NULL  0
#define  VFD_EV_RDY  1

#ifdef __cplusplus
extern "C" {
#endif

void VFD_CS_high(void);
void VFD_CS_low(void);
uint8_t VFD_BSY(void);
void VFD_RST_assert(void);
void VFD_RST_deassert(void);
void VFD_init(void);

uint8_t VFD_get_event(const vfd_descriptor *vfd);
void VFD_rst_event(uart_descriptor *vfd);

#ifdef __cplusplus
}
#endif

#endif
