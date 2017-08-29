/*
 * gpio.c
 *
 *  Created on: 2017��5��24��
 *      Author: Administrator
 */
#include <hw_gpio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <osal.h>
#include <ble_service.h>
#include <sps.h>
#include "ble_gap.h"
#include "gpio.h"
#include "trackpad.h"
#include "ICM2060x.h"
#include "akm09915.h"
#include "sys_socf.h"

#define GPIO_HARDERROR_PRINTF

/**
 * @fn      LED_Init
 * @brief   initialize led pin
 * @param   none
 * @return  none
 */
void LED_Init(void) {
  /* key POWER IO */
  *(volatile uint16_t *) (0x500030C6) = 0xff;            // p3 port IO 1.8V
  *(volatile uint16_t *) (0x500030C8) = 0xff;            // p4 port IO 1.8V
  hw_gpio_set_pin_function(HW_GPIO_PORT_4, HW_GPIO_PIN_5, HW_GPIO_MODE_OUTPUT,
                           HW_GPIO_FUNC_GPIO);
  KEEP_POWER_ON;

  /* led */
  hw_gpio_set_pin_function(HW_GPIO_PORT_3, HW_GPIO_PIN_0, HW_GPIO_MODE_OUTPUT,
                           HW_GPIO_FUNC_GPIO);
  LED1_ON;

}

/**
 * @fn      LED_task
 * @brief   led task function
 * @param   none
 * @return  none
 */
void LED_task(void *params) {
  OS_BASE_TYPE ret;
  uint32_t notif = 0;

  /* create CONNECT_timer */
  PRIVILEGED_DATA static OS_TIMER *CONNECT_timer;
  CONNECT_timer = OS_TIMER_CREATE("CONNECT_timer", OS_MS_2_TICKS(2000),
                                  OS_TIMER_SUCCESS,
                                  (void *) OS_GET_CURRENT_TASK(), led_cb);
  OS_ASSERT(CONNECT_timer);
  OS_TIMER_START(CONNECT_timer, OS_TIMER_FOREVER);

  /* create fusion_led_timer */
  PRIVILEGED_DATA static OS_TIMER *fusion_led_timer;
  fusion_led_timer = OS_TIMER_CREATE("fusion_led_timer", OS_MS_2_TICKS(3000),
                                     OS_TIMER_SUCCESS,
                                     (void *) OS_GET_CURRENT_TASK(),
                                     fusion_led_cb);
  OS_ASSERT(fusion_led_timer);

  /* create _timer */
  PRIVILEGED_DATA static OS_TIMER *device_not_calibration_timer;
  device_not_calibration_timer = OS_TIMER_CREATE("device_not_calibration_timer", OS_MS_2_TICKS(1000),
                                     OS_TIMER_SUCCESS,
                                     (void *) OS_GET_CURRENT_TASK(),
                                     calibration_error_timer_cb);
  OS_ASSERT(device_not_calibration_timer);

  for (;;) {
    ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif,
                              OS_TASK_NOTIFY_FOREVER);
    OS_ASSERT(ret == OS_OK);

    if (notif & BLE_CONNECT_LED_START_NOTIF) {
      OS_TIMER_START(CONNECT_timer, OS_TIMER_FOREVER);
    }

    if (notif & BLE_CONNECT_LED_STOP_NOTIF) {
      LED1_OFF;
      OS_TIMER_STOP(CONNECT_timer, OS_TIMER_FOREVER);
    }

    /* BLE advertising status */
    if (notif & BLE_CONNECT_NOTIF) {
      LED1_ON;
      OS_DELAY_MS(100);
      LED1_OFF;
    }

    /* home key Pull one time */
    if (notif & POWER_KEY_NOTIF) {
      LED1_ON;
      OS_DELAY_MS(80);
      LED1_OFF;
    }

    /* fusion mode led flash */
    if (notif & FUSION_LED_NOTIF) {
      LED1_ON;
      OS_DELAY_MS(80);
      LED1_OFF;
    }

    /* start fusion timer  */
    if (notif & START_FUSION_TIMER_NOTIF) {
      OS_TIMER_START(fusion_led_timer, OS_TIMER_FOREVER);
    }

    /* stop fusion timer  */
    if (notif & STOP_FUSION_TIMER_NOTIF) {
      OS_TIMER_STOP(fusion_led_timer, OS_TIMER_FOREVER);
    }

    /* device not calibration */
    if( notif & DEVICE_NOT_CALIBRATION_LED_NOTIF ){
      LED1_ON;
    }

    /* start START NOT CALIBRATION TIMER_NOTIF notif */
    if( notif & START_NOT_CALIBRATION_TIMER_NOTIF ){
      OS_TIMER_STOP(CONNECT_timer, OS_TIMER_FOREVER);
      OS_TIMER_START(device_not_calibration_timer, OS_TIMER_FOREVER);
    }
  }
}

/**
 * @brief   CONNECT_timer call back function
 * @param   none
 * @return  none
 */
void led_cb( OS_TIMER timer) {
  OS_TASK task = (OS_TASK) OS_TIMER_GET_TIMER_ID(timer);
  OS_TASK_NOTIFY(task, BLE_CONNECT_NOTIF, eSetBits);
}

/**
 * @brief   fusion led timer call back function
 * @param   none
 * @return  none
 */
void fusion_led_cb( OS_TIMER timer) {
  OS_TASK task = (OS_TASK) OS_TIMER_GET_TIMER_ID(timer);
  OS_TASK_NOTIFY(task, FUSION_LED_NOTIF, eSetBits);
}

/**
 * @brief   hardware error timer cb function
 * @param   none
 * @return  none
 */
void hardware_error_timer_cb( OS_TIMER timer ) {
  OS_TASK task = (OS_TASK) OS_TIMER_GET_TIMER_ID(timer);
  OS_TASK_NOTIFY(task, HARDWARE_ERROR_LED_NOTIF, eSetBits);
}

/**
 * @brief   HARDWARE ERROR LED NOTIF function
 * @param   none
 * @return  none
 */
void calibration_error_timer_cb( OS_TIMER timer ){
  OS_TASK task = (OS_TASK) OS_TIMER_GET_TIMER_ID(timer);
  OS_TASK_NOTIFY(task, DEVICE_NOT_CALIBRATION_LED_NOTIF, eSetBits);
}

/**
 * @brief   led flash
 * @param   numTime: the time to flash
 * @return  none
 */
void led_flash() {
  uint8_t iflash = 0;
  for (iflash = 0; iflash < 3; iflash++) {
    LED1_OFF;
    OS_DELAY_MS(100);
    LED1_ON;
    OS_DELAY_MS(50);
  }
  LED1_OFF;
}

uint8_t check_hardware_error(void) {
  uint8_t errorValue = 0;

  if( !IQS5xx_CheckVersion()){
    errorValue++;
    OS_DELAY_MS(1000);
    for( uint8_t ibit = 0 ; ibit < 5; ibit++ ){
      LED1_OFF;
      OS_DELAY_MS(300);
      LED1_ON;
      OS_DELAY_MS(300);
    }
     #ifdef GPIO_HARDERROR_PRINTF
     printf( "  gpio_IQS5_error\n" );
     #endif
  }

  if( !readICM_whoIam() ){
    errorValue++;
    OS_DELAY_MS(1000);
    for( uint8_t ibit = 0 ; ibit < 10; ibit++ ){
      LED1_OFF;
      OS_DELAY_MS(300);
      LED1_ON;
      OS_DELAY_MS(300);
    }
    #ifdef GPIO_HARDERROR_PRINTF
    printf( " gpio_readICM_error \n" );
    #endif
  }

  if( !readAKM_whoIAM() ){
    errorValue++;
    OS_DELAY_MS(1000);
    for( uint8_t ibit = 0 ; ibit < 15; ibit++ ){
      LED1_OFF;
      OS_DELAY_MS(300);
      LED1_ON;
      OS_DELAY_MS(300);
    }
    #ifdef GPIO_HARDERROR_PRINTF
    printf( " gpio_readAKM_error \n" );
    #endif
  }
  if( errorValue ){
    #ifdef GPIO_HARDERROR_PRINTF
    printf( " gpio_power off \n" );
    #endif
    LED1_OFF;
    KEEP_POWER_OFF;
    return 0;
  }
  return 1;
}

