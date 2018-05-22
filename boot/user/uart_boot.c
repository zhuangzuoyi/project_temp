#include "uart_boot.h"

#include  "ringBufS.h"
#include "stdio.h"
#define BOOTLOADER_DATA_MAX  1024

#define IS_DEBUG  1

#if IS_DEBUG
#define boot_debug(...)     printf(__VA_ARGS__)
#elif
#define boot_debug(...)
#endif

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
                boot_debug("get head\r\n");
                if (current_byte == 0xaa)
                {
                    BT_CmdDecodeState = RX_DECODE_CMD_LEN_H;
                    bootloader_check_sum = 0;
                }
                break;

            case RX_DECODE_CMD_LEN_H:
                    bootloader_check_sum += current_byte;
                    boot_debug("get cmd len h\r\n");
                    bootloader_data_len = current_byte<<8;
                    BT_CmdDecodeState = RX_DECODE_CMD_LEN_L;

                break;

            case RX_DECODE_CMD_LEN_L:
                bootloader_check_sum += current_byte;
                BT_CmdDecodeState = RX_DECODE_CMD_OPCODE; //next state
                bootloader_data_len += current_byte;
                boot_debug("get cmd len l,The len is:%d\r\n",bootloader_data_len);
                break;

            case RX_DECODE_CMD_OPCODE:
                bootloader_check_sum += current_byte;
                bootloader_cmd = current_byte;
                if(bootloader_data_len !=0)
                BT_CmdDecodeState = RX_DECODE_CMD_DATA; 
                else
                BT_CmdDecodeState = RX_DECODE_CMD_CHECKSUM; 
                bootloader_data_index=0;
                boot_debug("get opcode:%x\r\n",bootloader_cmd);
              break;
            case RX_DECODE_CMD_DATA:
                bootloader_check_sum += current_byte;
                bootloader_data_len--;
                bootloader_data[bootloader_data_index++] = current_byte;
                if (bootloader_data_len == 0) //no data remained?
                {
                    BT_CmdDecodeState = RX_DECODE_CMD_CHECKSUM; //yes, next mode: checksum
                    boot_debug("get all data,data index is:%d\r\n",bootloader_data_index);
                }
              
                break;

            case RX_DECODE_CMD_CHECKSUM:
                if ((uint8_t) (bootloader_check_sum + current_byte) == 0) {
                    bootloader_cmd_decode_flag = 1;
                    boot_debug("checksum ok\r\n");
                } else {
                  boot_debug("checksum faile,%x\r\n",bootloader_check_sum + current_byte);
                }
                BT_CmdDecodeState = RX_DECODE_CMD_START_AA;
                
                break;
            default:
                break;
        }

    }
}


 uint8_t calculateChecksum(uint8_t* dat, uint16_t len)
{
    uint8_t checksum = 0;
    uint16_t i=0;
    for(i=0;i<len;i++)
    {
        checksum += *dat;
        dat++;
    }
    checksum = ~checksum + 1;
    return checksum;
}


void bootloader_return_version(void)
{
  uint8_t dat[8];
  dat[0] = 0xaa;
  dat[1] = 0;
  dat[2] = 3;
  dat[3] = bootloader_cmd;
  dat[4] = BOOT_VERSION;
  dat[5] = BOOT_SUBVERSION;
  dat[6] = BOOT_REVISION;
  dat[7] = calculateChecksum(&dat[1],6);
  HAL_UART_Transmit(&huart2, dat, 8,1000);
}

void bootloader_return_mcu_model(void)
{
  uint8_t dat[20],i=sizeof(MCU_MODEL);
  
    dat[0] = 0xaa;
    dat[1] = 0;
    dat[2] = sizeof(MCU_MODEL);
    dat[3] = bootloader_cmd;
    dat[4] = 's';
    dat[5] = 't';
    dat[6] =  'm';
    dat[7] = '3';
    dat[8] = '2';
    dat[9] = 'f';
   dat[10] = '1';
   dat[11] = '0';
   dat[12] = '3';
   dat[13] = 'r';
   dat[14] = 'b';
//  sprintf(&dat[4],"%s",MCU_MODEL);
  
  dat[15] = calculateChecksum(&dat[1],14);
  HAL_UART_Transmit(&huart2, dat,16,1000);
}

void boot_msg_decode(uint8_t cmd)
{
  switch(cmd)
  {
  case BOOT_CMD_GET_BOOT_VER:
    bootloader_return_version();
    break;
  case BOOT_CMD_GET_MCU_MODEL:
    bootloader_return_mcu_model();
    break;
  case BOOT_CMD_GET_APP_START_ADD:
    break;
  }
}

void boot_flash_cmd(uint8_t cmd)
{
  switch(cmd)
  {
    case 0x00:
      boot_debug("erea app\r\n");
      break;
    case 0x01:
      boot_debug("write flash\r\n");
      break;
  }
}

void bootloader_cmd_decode(void)
{
  switch(bootloader_cmd)
  {
    case BOOT_CMD_BOOT_MSG:
      boot_msg_decode(bootloader_data[0]);
     break;
    case BOOT_CMD_APP_MSG:
      
    break;  
    case BOOT_CMD_FLASH:
      
    break;
  }
}


void bootloader_task(void)
{
  bootloader_cmd_handler();
  if(bootloader_cmd_decode_flag)
  {
    bootloader_cmd_decode();
    bootloader_cmd_decode_flag = 0;
  }
}






