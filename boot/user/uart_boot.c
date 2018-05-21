#include "uart_boot.h"

#include  "ringBufS.h"

#define BOOTLOADER_DATA_MAX  1024
ringBufS my_ringBuffer;
RX_DECODE_MODE  BT_CmdDecodeState;
uint8_t bootloader_cmd;
uint8_t bootloader_data_len;
uint8_t bootloader_check_sum=0;
uint8_t received_temp=0;
extern UART_HandleTypeDef huart2;
uint8_t bootloader_data_index=0;
uint8_t bootloader_data[BOOTLOADER_DATA_MAX];
uint8_t bootloader_cmd_decode_flag=0;


void bootloader_init(void)
{
  HAL_UART_Receive_IT(&huart2, &received_temp, 1);
  ringBufS_init (&my_ringBuffer);
  bootloader_cmd = 0;
  bootloader_data_len = 0;
  bootloader_check_sum=0;
}


void bootloader_buf_put(uint8_t dat)
{
  ringBufS_put(&my_ringBuffer, dat);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  if(huart->Instance == USART2)
  {
    bootloader_buf_put(received_temp);
    HAL_UART_Receive_IT(&huart2, &received_temp, 1);
    
  }
}

int bootloader_buf_get(void)
{
  return ringBufS_get(&my_ringBuffer);
}

int bootloader_buf_is_full(void)
{
  return ringBufS_full(&my_ringBuffer);
}


void bootloader_cmd_handler(void) {
    int dat_temp;
    uint8_t current_byte;

    while (!ringBufS_empty(&my_ringBuffer)) {
        dat_temp = bootloader_buf_get();
        
        if(dat_temp ==-1)
          break;
        else
          current_byte = (uint8_t) dat_temp;
        
        switch (BT_CmdDecodeState) {
            case RX_DECODE_CMD_START_AA:
                if (current_byte == 0xaa)
                    BT_CmdDecodeState = RX_DECODE_CMD_LEN_H;
                break;

            case RX_DECODE_CMD_LEN_H:
                    bootloader_data_len = current_byte<<8;
                    BT_CmdDecodeState = RX_DECODE_CMD_LEN_L;

                break;

            case RX_DECODE_CMD_LEN_L:
                BT_CmdDecodeState = RX_DECODE_CMD_OPCODE; //next state
                bootloader_data_len += current_byte;
                break;

            case RX_DECODE_CMD_OPCODE:
                bootloader_cmd = current_byte;
                BT_CmdDecodeState = RX_DECODE_CMD_DATA; 
                bootloader_data_index=0;
              break;
            case RX_DECODE_CMD_DATA:
                bootloader_check_sum += current_byte;
                bootloader_data_len--;
                bootloader_data[bootloader_data_index++] = current_byte;
                if (bootloader_data_len == 0) //no data remained?
                    BT_CmdDecodeState = RX_DECODE_CMD_CHECKSUM; //yes, next mode: checksum
              
                break;

            case RX_DECODE_CMD_CHECKSUM:
                if ((uint8_t) (bootloader_check_sum + current_byte) == 0) {
                    bootloader_cmd_decode_flag = 1;
                } else {
                }
                //BT_CmdDecodeState = RX_DECODE_CMD_SYNC_AA;
              
                break;
            default:
                break;
        }

    }
}


void bootloader_task(void)
{
  bootloader_cmd_handler();
  if(bootloader_cmd_decode_flag)
  {
    bootloader_cmd_decode_flag = 0;
  }
}