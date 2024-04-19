
#include "../include/mailbox.h"
// #include "general.h"
#include "../include/uart.h"

#include <stdint.h>
/* mailbox message buffer */

volatile uint32_t __attribute__((aligned(16))) mbox_buf[36];

/**
 * Make a mailbox call. Returns 0 on failure, non-zero on success
 */
int mbox_call(unsigned char ch) {
  return mbox_call_user_buffer(ch, (unsigned int *)mbox_buf);
}
/**
 * @param ch: 0~8, 8 for property
 * @param mbox: a pointer to the buffer from user space.
 *              caller should fill it as documented.
 *              If the there is an output from mail box, the output is also
 * placed somewhere in the buffer
 * @return 0 on failure, non-zero on success
 */
int mbox_call_user_buffer(unsigned char ch, unsigned int *mbox) {
  uint32_t temp =
      ((((uint32_t)((uint64_t)mbox)) & ~0xF) // & ~0xF clears lower 4 bits
       | (ch & 0xF) // & 0x0000000F clears upper 28 bits
      );            // 28 bits(MSB) for value, 4 bits for the channel

  // Wait until mailbox is not full (busy)
  while (*MBOX_STATUS & MBOX_FULL)
    ;

  // Write buffer address to mobx_write
  *MBOX_WRITE = temp;

  // Wait while it's empty
  while (*MBOX_STATUS & MBOX_EMPTY)
    ;

  // Check if the value is the same as the one wrote into MBOX_WRITE
  if (*MBOX_READ == temp)
    /* is it a valid successful response? */
    return mbox[1] == MBOX_RESPONSE;
  else
    return 0;
}

int mbox_board_rev(uint32_t *board_reviion) {
  int ret = 0;

  mbox_buf[0] = 7 * 4; // buffer size in bytes, 7 for 7 elements to
                       // MBOX_END_TAG; 4 for each elements is 4 bytes (u32)
  mbox_buf[1] = MBOX_REQUEST_CODE;       // fixed code
                                         // tags begin
  mbox_buf[2] = MBOX_GET_BOARD_REVISION; // tag identifier
  mbox_buf[3] = 4;                       // response length
  mbox_buf[4] = MBOX_TAG_REQUEST_CODE;   // fixed code
  mbox_buf[5] = 0;                       // output buffer, clear it here
                                         // tags end
  mbox_buf[6] = MBOX_END_TAG;

  // Send mailbox request
  ret = mbox_call(MBOX_CH_PROP);
  *board_reviion = mbox_buf[5];
  return ret;
}

int mbox_arm_mem_info(uint32_t **base_addr, uint32_t *size) {
  int ret = 0;

  mbox_buf[0] = 8 * 4; // buffer size in bytes, 8 for 8 elements to
                       // MBOX_END_TAG; 4 for each elements is 4 bytes (u32)
  mbox_buf[1] = MBOX_REQUEST_CODE;     // fixed code
                                       // tags begin
  mbox_buf[2] = MBOX_GET_ARM_MEMORY;   // tag identifier
  mbox_buf[3] = 8;                     // response length
  mbox_buf[4] = MBOX_TAG_REQUEST_CODE; // fixed code
  mbox_buf[5] = 0;                     // output buffer 0, clear it here
  mbox_buf[6] = 0;                     // output buffer 1, clear it here
                                       // tags end
  mbox_buf[7] = MBOX_END_TAG;

  // Send mailbox request
  ret = mbox_call(MBOX_CH_PROP); // message passing procedure call
  *base_addr = (uint32_t *)((
      uint64_t)mbox_buf[5]); // cast to u64 cuz uint32_t* takes 64 bits
  *size = mbox_buf[6];
  return ret;
}
