/*
 * key.c
 *
 *  Created on: 2017年5月24日
 *      Author: Administrator
 */
#include "key.h"
#include <hw_gpio.h>
#include <stdio.h>
#include <string.h>
#include <osal.h>
#include "axis.h"
#include "AHRS_alg.h"

uint32_t SHUTDOWN_KEY_ON_FLAG = 0;

/**
 * @fn      key_task
 * @brief   key task function
 * @param   none
 * @return  none
 */
//void key_task(void *params) {
//  uint8_t keyValue = 0;
//  OS_QUEUE_CREATE(keyQueue, sizeof(keyValue), 1);
//  OS_ASSERT(keyQueue);
//  for (;;) {
//    printf("key task:\n\r");
//    keyValue = Key_Scan();
//    printf("keyvalue = 0x%x \n\r", keyValue);
//
//    if (uxQueueSpacesAvailable(keyQueue) >= 1) {
//      if ( OS_QUEUE_PUT(keyQueue, &keyValue, 0) == OS_OK) {
//        printf("OS_QUEUE_PUT success = \n\r");
//      } else {
//
//      }
//    } else {
//      printf("OS_QUEUE_PUT fail = \n\r");
//    }
//    OS_DELAY_MS(16);
//  }
//}

/**
 * @fn      key_init
 * @brief   key initialize
 * @param   none
 * @return  none
 */
void key_init(void) {

  /* KEY_X0: */
  hw_gpio_set_pin_function(HW_GPIO_PORT_3, HW_GPIO_PIN_2,
                           HW_GPIO_MODE_INPUT_PULLUP, HW_GPIO_FUNC_GPIO);

  /* KEY_X1: */
  hw_gpio_set_pin_function(HW_GPIO_PORT_4, HW_GPIO_PIN_7,
                           HW_GPIO_MODE_INPUT_PULLUP, HW_GPIO_FUNC_GPIO);

  /* KEY_Y1: */
  hw_gpio_set_pin_function(HW_GPIO_PORT_4, HW_GPIO_PIN_3,
                           HW_GPIO_MODE_INPUT_PULLUP, HW_GPIO_FUNC_GPIO);

  /* key shutdown DET */
  hw_gpio_set_pin_function(HW_GPIO_PORT_4, HW_GPIO_PIN_6,
                           HW_GPIO_MODE_INPUT_PULLUP, HW_GPIO_FUNC_GPIO);

  /* touchpad RDY */
  hw_gpio_set_pin_function(HW_GPIO_PORT_4, HW_GPIO_PIN_1,
                           HW_GPIO_MODE_INPUT_PULLDOWN, HW_GPIO_FUNC_GPIO);

  /* AK_DRDY */
  hw_gpio_set_pin_function(HW_GPIO_PORT_3, HW_GPIO_PIN_6,
                           HW_GPIO_MODE_INPUT_PULLUP, HW_GPIO_FUNC_GPIO);

  /* ICM_INT */
  hw_gpio_set_pin_function(HW_GPIO_PORT_3, HW_GPIO_PIN_3,
                           HW_GPIO_MODE_INPUT_PULLUP, HW_GPIO_FUNC_GPIO);
}

/**
 * @fn      Key_Scan
 * @brief   check key status
 * @param   none
 * @return  none
 */
uint8_t Key_Scan(void) {
  static uint8_t key_bj_counter  = 0;
  static uint8_t key_menu_counter = 0;
  static uint8_t key_return_counter = 0;
  static uint8_t key_swich_power_counter = 0;
  static uint8_t attitudeDoneflag = 0;
  uint8_t key_value = 0;

  /* key bj */
  if (!READ_KEY_BJ) {
    key_bj_counter++;
    if( key_bj_counter > KEY_SHAKE_NUM ){
       key_value |= 1 << KEY_BJ;
       key_bj_counter--;                   // 防溢出
    }
  }else{
    key_bj_counter = 0;
  }

  /* key menu */
  if (!READ_KEY_MENU) {
    key_menu_counter++;
    if( key_menu_counter  > KEY_SHAKE_NUM ){
       key_value |= 1 << KEY_MENU;
       key_menu_counter--;
    }
  }else{
      key_menu_counter = 0;
  }

  /* key return */
  if (!READ_KEY_RETURN) {
    key_return_counter++;
    if( key_return_counter > KEY_SHAKE_NUM ){
      key_value |= 1 << KEY_RETURN;
      key_return_counter--;
    }
  }else{
     key_return_counter = 0;
  }

  /* key power*/
  if (!READ_SHUTDOWN_DET) {
    key_swich_power_counter++;
    if( key_swich_power_counter > KEY_SHAKE_NUM ){
       key_value |= 1 << KEY_SWICH_POWER;
       key_swich_power_counter--;
    }
    if (SHUTDOWN_KEY_ON_FLAG == TRIGGER_AUTO_RESET_POSE_TIME) {
      InitAttitude();
    } else if (SHUTDOWN_KEY_ON_FLAG > TRIGGER_AUTO_RESET_POSE_TIME + 1) {
      if (IsInitAttitudeDone() && ( attitudeDoneflag == 0 )) {
        attitudeDoneflag =1;
        key_value |= 0x80;
      }
    }
    SHUTDOWN_KEY_ON_FLAG++;
  } else {
    if (SHUTDOWN_KEY_ON_FLAG > 5) {
      OS_TASK_NOTIFY(led_handle, POWER_KEY_NOTIF, eSetBits);
    }
    SHUTDOWN_KEY_ON_FLAG = 0;
    attitudeDoneflag =0;
    key_swich_power_counter = 0;
  }
  return key_value;
}

uint8_t Daydream_Key_Scan(void) {
  static uint8_t key_bj_counter  = 0;
  static uint8_t key_menu_counter = 0;
  static uint8_t key_return_counter = 0;
  static uint8_t key_swich_power_counter = 0;
  uint8_t key_value = 0;

  /* key bj */
  if (!READ_KEY_BJ) {
    key_bj_counter++;
    if( key_bj_counter > KEY_SHAKE_NUM ){
       key_value |= 1 << 0;
       key_bj_counter--;                   // 防溢出
    }
  }else{
    key_bj_counter = 0;
  }

  /* key menu */
  if (!READ_KEY_MENU) {
    key_menu_counter++;
    if( key_menu_counter  > KEY_SHAKE_NUM ){
       key_value |= 1 << 2;
       key_menu_counter--;
    }
  }else{
      key_menu_counter = 0;
  }

  /* key return */
  if (!READ_KEY_RETURN) {
    key_return_counter++;
    if( key_return_counter > KEY_SHAKE_NUM ){
      key_value |= 1 << 4;
      key_return_counter--;
    }
  }else{
     key_return_counter = 0;
  }

  /* key power*/
  if (!READ_SHUTDOWN_DET) {
    key_swich_power_counter++;
    if( key_swich_power_counter > KEY_SHAKE_NUM ){
       key_value |= 1 << 1;
       key_swich_power_counter--;
    }
    if (SHUTDOWN_KEY_ON_FLAG == TRIGGER_AUTO_RESET_POSE_TIME) {
      InitAttitude();
    } else if (SHUTDOWN_KEY_ON_FLAG > TRIGGER_AUTO_RESET_POSE_TIME + 1) {
      if (IsInitAttitudeDone()) {
        key_value |= 0x80;
      }
    }
    SHUTDOWN_KEY_ON_FLAG++;
  } else {
    if (SHUTDOWN_KEY_ON_FLAG > 5) {
      OS_TASK_NOTIFY(led_handle, POWER_KEY_NOTIF, eSetBits);
    }
    SHUTDOWN_KEY_ON_FLAG = 0;
    key_swich_power_counter = 0;
  }
  return key_value;
}

