/*
 * pxp_flash.c
 *
 *  Created on: 2017Äê6ÔÂ10ÈÕ
 *      Author: Administrator
 */

#include "pxp_flash.h"

//#define PXP_FLASH_PRINTF

/**
 * @brief   read data from flash in the addr_offset
 * @param   readBuf:     read data store buffer
 *          addr_offset: read data offset addr
 *          length:      read data length
 * @return  1:read success; 0:read fail
 */
uint8_t read_flash_data(uint8_t *readBuf, uint16_t addr_offset, uint16_t length) {
  uint16_t res_length = 0;
  uint32_t addr_read = DATA_FLASH_ADDRESS_START + addr_offset;

  res_length = ad_flash_read(addr_read, readBuf, length);
  if (res_length > 0) {
#ifdef PXP_FLASH_PRINTF
    printf(" read data =  ");
    for (uint16_t ibit = 0; ibit < res_length; ibit++) {
      printf(" 0x%02x ", readBuf[ibit]);
    }
    printf("\n\r");
#endif
    return 1;
  }
  return 0;
}

/**
 * @brief   write data to flash in the addr_offset
 * @param   writeBuf:    the data to write
 *          addr_offset: write data offset addr
 *          length:      write data length
 * @return  1:read success; 0:read fail
 */
uint8_t write_flash_data(uint8_t *writeBuf, uint16_t addr_offset, uint16_t length) {
  uint16_t res_length = 0;
  uint32_t addr_write = DATA_FLASH_ADDRESS_START + addr_offset;

  ad_flash_erase_region(DATA_FLASH_ADDRESS_START, FLASH_ERASE_MIN_SIZE_4K);
  res_length = ad_flash_write(addr_write, writeBuf, length);
  if (res_length > 0) {
    return 1;
  }
  return 0;
}
