/*
 * gpio.h
 *
 *  Created on: 2017Äê5ÔÂ24ÈÕ
 *      Author: Administrator
 */

#ifndef GPIO_INC_GPIO_H_
#define GPIO_INC_GPIO_H_

#include <hw_gpio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <osal.h>
#include <ble_service.h>
#include <sps.h>
#include "ble_gap.h"
#include "task_config.h"

/**
 * led write define
 */
#define LED1_OFF                               hw_gpio_set_inactive(HW_GPIO_PORT_3, HW_GPIO_PIN_0)
#define LED1_ON                                hw_gpio_set_active(HW_GPIO_PORT_3, HW_GPIO_PIN_0)

#define KEEP_POWER_ON                          hw_gpio_set_active(HW_GPIO_PORT_4, HW_GPIO_PIN_5)
#define KEEP_POWER_OFF                         hw_gpio_set_inactive(HW_GPIO_PORT_4, HW_GPIO_PIN_5)

#define LED_NOTIF                              (1<<1)
#define POWER_KEY_NOTIF                        (1<<2)
#define BLE_CONNECT_NOTIF                      (1<<3)
#define BLE_CONNECT_LED_START_NOTIF            (1<<4)
#define BLE_CONNECT_LED_STOP_NOTIF             (1<<5)
#define FUSION_LED_NOTIF                       (1<<6)
#define START_FUSION_TIMER_NOTIF               (1<<7)
#define STOP_FUSION_TIMER_NOTIF                (1<<8)
#define HARDWARE_ERROR_LED_NOTIF               (1<<9)
#define START_HARDWARE_ERROR_TIMER_NOTIF       (1<<10)
#define DEVICE_NOT_CALIBRATION_LED_NOTIF       (1<<11)
#define START_NOT_CALIBRATION_TIMER_NOTIF      (1<<12)

/**
 *  Function declaration
 */
void LED_Init(void);
void led_cb(OS_TIMER timer);
void fusion_led_cb( OS_TIMER timer);
void LED_task(void *params);
void led_flash();
void hardware_error_timer_cb( OS_TIMER timer );
void calibration_error_timer_cb( OS_TIMER timer );
uint8_t check_hardware_error(void);

#endif /* GPIO_INC_GPIO_H_ */
