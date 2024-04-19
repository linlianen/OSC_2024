#ifndef __MBOX_H
#define __MBOX_H

#include "stdint.h"

#define MBOX_REQUEST 0

#define MMIO_BASE 0x3F000000
/* channels */
#define MBOX_CH_POWER 0
#define MBOX_CH_FB 1
#define MBOX_CH_VUART 2
#define MBOX_CH_VCHIQ 3
#define MBOX_CH_LEDS 4
#define MBOX_CH_BTNS 5
#define MBOX_CH_TOUCH 6
#define MBOX_CH_COUNT 7
#define MBOX_CH_PROP 8


#define VIDEOCORE_MBOX (MMIO_BASE + 0x0000B880)
#define MBOX_READ ((volatile uint32_t *)(VIDEOCORE_MBOX + 0x00))
#define MBOX_POLL ((volatile uint32_t *)(VIDEOCORE_MBOX + 0x10))
#define MBOX_SENDER ((volatile uint32_t *)(VIDEOCORE_MBOX + 0x14))
#define MBOX_STATUS ((volatile uint32_t *)(VIDEOCORE_MBOX + 0x18))
#define MBOX_CONFIG ((volatile uint32_t *)(VIDEOCORE_MBOX + 0x1C))
#define MBOX_WRITE ((volatile uint32_t *)(VIDEOCORE_MBOX + 0x20))
#define MBOX_RESPONSE 0x80000000
#define MBOX_FULL 0x80000000
#define MBOX_EMPTY 0x40000000

#define MBOX_GET_BOARD_REVISION                                                \
  0x00010002 // request_length=0, response_length=4, response_value={u32 board
             // revision}
#define MBOX_GET_ARM_MEMORY                                                    \
  0x00010005 // request_length=0, response_length=8, response_value={u32 base
             // address in bytes, u32 size in bytes}
#define MBOX_REQUEST_CODE 0x00000000
#define MBOX_REQUEST_SUCCEED 0x80000000
#define MBOX_REQUEST_FAILED 0x80000001
#define MBOX_TAG_REQUEST_CODE 0x00000000
#define MBOX_END_TAG 0x00000000
/* tags */
#define MBOX_TAG_GETSERIAL 0x10004
#define MBOX_TAG_LAST 0

int mbox_board_rev(uint32_t *board_reviion);
int mbox_arm_mem_info(uint32_t **base_addr, uint32_t *size);

int mbox_call_user_buffer(unsigned char ch, unsigned int *mbox);

#endif /* __MBOX_H */
