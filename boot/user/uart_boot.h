#ifndef __UART_BOOT_H__
#define __UART_BOOT_H__


#include "stm32f1xx_hal.h"


enum{
  BOOT_CMD_GET_BOOT_VER=0,
  BOOT_CMD_GET_MCU_MODEL,
  BOOT_CMD_GET_APP_START_ADD,
};

typedef enum {
	RX_DECODE_CMD_START_AA,
	RX_DECODE_CMD_LEN_H,
        RX_DECODE_CMD_LEN_L,
        RX_DECODE_CMD_OPCODE,
	RX_DECODE_CMD_DATA,
	RX_DECODE_CMD_CHECKSUM
} RX_DECODE_MODE;

void bootloader_init(void);
void bootloader_task(void);

#endif

