/*
 * pxp_flash.h
 *
 *  Created on: 2017Äê6ÔÂ10ÈÕ
 *      Author: Administrator
 */

#ifndef PXP_FLASH_INC_PXP_FLASH_H_
#define PXP_FLASH_INC_PXP_FLASH_H_

#include <ad_flash.h>
#include <sdk_defs.h>

#define  FLASH_SIZE_MAX                               0x100000
#define  FLASH_ERASE_MIN_SIZE_4K                      0x1000                                                    // 4k
#define  DATA_FLASH_ADDRESS_START                     (0x100000 - 0x1000)


/**
 *  Function declaration
 */
uint8_t read_flash_data(uint8_t *readBuf, uint16_t addr_offset, uint16_t length);
uint8_t write_flash_data(uint8_t *writeBuf,uint16_t addr_offset,uint16_t length);
void flash_task(void *params);


#endif /* PXP_FLASH_INC_PXP_FLASH_H_ */
