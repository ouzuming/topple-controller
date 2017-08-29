/*
 * key.h
 *
 *  Created on: 2017年5月24日
 *      Author: Administrator
 */

#ifndef KEY_INC_KEY_H_
#define KEY_INC_KEY_H_

#include <hw_gpio.h>
#include "gpio.h"

/**
 * key read define
 */
#define READ_KEY_BJ                                    hw_gpio_get_pin_status( HW_GPIO_PORT_4, HW_GPIO_PIN_3)
#define  READ_KEY_RETURN                               hw_gpio_get_pin_status( HW_GPIO_PORT_4, HW_GPIO_PIN_7)
#define  READ_KEY_MENU                                 hw_gpio_get_pin_status( HW_GPIO_PORT_3, HW_GPIO_PIN_2)
#define READ_SHUTDOWN_DET                              hw_gpio_get_pin_status( HW_GPIO_PORT_4, HW_GPIO_PIN_6)

#define READ_TOUCHPAD_RDY                              hw_gpio_get_pin_status( HW_GPIO_PORT_4, HW_GPIO_PIN_1)
#define READ_AK_DRDY                                   hw_gpio_get_pin_status( HW_GPIO_PORT_3, HW_GPIO_PIN_6)               //"H" 有数据读
#define READ_ICM_INT                                   hw_gpio_get_pin_status( HW_GPIO_PORT_3, HW_GPIO_PIN_3)

#define KEY_BJ                                         0
#define KEY_MENU                                       1
#define KEY_RETURN                                     2
#define KEY_SWICH_POWER                                3

#define TRIGGER_AUTO_RESET_POSE_TIME                   70

#define KEY_SHAKE_NUM                                  2

#define DAYDREAM_MODE                                  1
#define I3VR_MODE                                      0

/**
 *  Function declaration
 */
void key_task(void *params);
void key_init(void);
uint8_t Key_Scan(void);
uint8_t Daydream_Key_Scan(void);

#endif /* KEY_INC_KEY_H_ */
