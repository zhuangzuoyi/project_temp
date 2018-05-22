#ifndef __UART_BOOT_H__
#define __UART_BOOT_H__


#include "stm32f1xx_hal.h"

#define BOOT_VERSION                      0L              /**< major version number */
#define BOOT_SUBVERSION                   0L              /**< minor version number */
#define BOOT_REVISION                     1L              /**< revise version number */

#define MCU_MODEL                         "STM32F103rb"

enum{
  BOOT_CMD_BOOT_MSG=0x00,
  BOOT_CMD_APP_MSG,
  BOOT_CMD_FLASH
};

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

