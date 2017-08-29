/*
 * fusion.h
 *
 *  Created on: 2017Äê7ÔÂ6ÈÕ
 *      Author: Administrator
 */

#ifndef FUSION_INC_FUSION_H_
#define FUSION_INC_FUSION_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <osal.h>
#include <ble_service.h>
#include <sps.h>
#include "ble_gap.h"
#include "akm09915.h"
#include <hw_gpio.h>
#include <sps.h>
#include "task_config.h"
#include "msg_queues.h"
#include "queue.h"

extern uint8_t g_FUSION_MODE_FLAG;
#define HEAD_STN_B1                    0xA5

#define FUSION_NOTIF                   (1<<0)
#define COM_A1_NOTIF                   (1<<1)
#define COM_A2_NOTIF                   (1<<2)
#define COM_A3_NOTIF                   (1<<3)
#define COM_A4_NOTIF                   (1<<4)
#define COM_A5_NOTIF                   (1<<5)
#define COM_A6_NOTIF                   (1<<6)
#define COM_A7_NOTIF                   (1<<7)
#define COM_A8_NOTIF                   (1<<8)

#define HEAD_BIT                        0
#define LEN_BIT                         1
#define COMMAND_BIT                     2
#define COMEFF_BIT                      3

#define COM_A1                          0xA1
#define COM_A2                          0xA2
#define COM_A3                          0xA3
#define COM_A4                          0xA4
#define COM_A5                          0xA5
#define COM_A6                          0xA6
#define COM_A7                          0xA7
#define COM_A8                          0xA8


#define PC_ST                           0x80
#define CONTROLER_ST                    0x40
#define CONTROLER_RESPONSE_COMMAND      0xB2
#define CONTROLER_CALIBRATION_COMMAND   0xB3
#define RESPONSE_ERROR_1                0xF1

#define PACKAGE_LEN                     20
#define FUSOIN_PACKAGE_LEN              145
#define CALIBRATION_DATA_LEN            144
#define WRITE_CALIBRATION_FLAG          0xAA

//OS_MUTEX sps_ccc_event;

void fusion_task(void *params);
void fusion_cb( OS_TIMER timer);
void sps_rx_data_cb(ble_service_t *svc, uint16_t conn_idx, const uint8_t *value,uint16_t length);
void sps_tx_done_cb(ble_service_t *svc, uint16_t conn_idx, uint16_t length);
void sps_set_flow_control_cb(ble_service_t *svc, uint16_t conn_idx, sps_flow_control_t value);
void response_command( uint8_t snValue, uint8_t commandValue );
void response_calibration_data(uint8_t snValue, uint8_t *calibrationData, uint8_t calibrationLength );

#endif /* FUSION_INC_FUSION_H_ */
