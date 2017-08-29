/*
 * lowPower.c
 *
 *  Created on: 2017Äê6ÔÂ1ÈÕ
 *      Author: Administrator
 */

#include "lowPower.h"
#include "AHRS_alg.h"
#include <string.h>
#include <osal.h>
#include "gpio.h"
#include "task_config.h"
#include "notify.h"
#include "axis.h"
#include "fusion.h"

//#define LOWPOWER_PRINTF


/**
 * @fn      power_task
 * @brief   low power task
 * @param   none
 * @return  none
 */
void power_task(void *params) {
  OS_BASE_TYPE ret;
  uint32_t notif;

  PRIVILEGED_DATA static OS_TIMER *counter_timer;
  counter_timer = OS_TIMER_CREATE("counter_timer", OS_MS_2_TICKS(6000),
                                  OS_TIMER_SUCCESS,
                                  (void *) OS_GET_CURRENT_TASK(),
                                  counter_time_cb);

  OS_ASSERT(counter_timer);
  //OS_TIMER_START(counter_timer, OS_TIMER_FOREVER);
  OS_DELAY_MS(50);
  for (;;) {
    ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif,
                              OS_TASK_NOTIFY_FOREVER);
    OS_ASSERT(ret == OS_OK);

    if (notif & COUNTER_NOTIF) {
        #ifdef LOWPOWER_PRINTF
        printf("power_task_notif & COUNTER_NOTIF\n");
        #endif
      if (GetSteadyStateTime( ENTER_LOWPOWER_TIME)) {
        /* get in low power mode */
        #ifdef LOWPOWER_PRINTF
        printf("lowpower_GetSteadyStateTime\n");
        #endif
        KEEP_POWER_OFF;
      }
    }
    /* BLE ADV timeout */
    if (notif & POWEROFF_TIME_COUNTER_NOTIF) {
      #ifdef LOWPOWER_PRINTF
      printf("lowpower_task_POWEROFF_TIME_COUNTER_NOTIF\n");
      #endif
      KEEP_POWER_OFF;
    }

    if (notif & CCC_NOTIFICATIONS_NOTIF ) {
      #ifdef LOWPOWER_PRINTF
      printf("lowpower_task_CCC_NOTIFICATIONS_NOTIF\n");
      #endif
     KEEP_POWER_OFF;
    }

    if (notif & POWER_OFF_COMMAND_NOTIF ) {
      #ifdef LOWPOWER_PRINTF
      printf("lowpower_task_POWER_OFF_COMMAND_NOTIF\n");
      #endif
     KEEP_POWER_OFF;
    }

    if( notif & SUTOA_START_NOTIF ){
        #ifdef LOWPOWER_PRINTF
        printf("lowpower_SUTOA_START_NOTIF\n");
        #endif
        OS_TIMER_STOP(counter_timer, OS_TIMER_FOREVER);
        OS_TASK_NOTIFY(notify_handle, STOP_TIME_NOTIF, eSetBits);
        OS_TASK_NOTIFY(axis_handle, AXIS_TIMER_STOP_NOTIF, eSetBits);

    }

    if( notif & SUTOA_END_NOTIF ){
        #ifdef LOWPOWER_PRINTF
        printf("lowpower_SUTOA_END_NOTIF\n");
        #endif
        OS_TIMER_START(counter_timer, OS_TIMER_FOREVER);
        OS_TASK_NOTIFY(notify_handle, START_TIME2_NOTIF, eSetBits);
    }

    if( notif & COUNTER_TEME_START_NOTIF ){
        OS_TIMER_START(counter_timer, OS_TIMER_FOREVER);
    }

    if( notif & COUNTER_TEME_STOP_NOTIF ){
        OS_TIMER_STOP(counter_timer, OS_TIMER_FOREVER);
    }
  }
}

/**
 * @fn      led callback function
 * @brief   initialize led pin
 * @param   none
 * @return  none
 */
void counter_time_cb( OS_TIMER timer) {
  OS_TASK task = (OS_TASK) OS_TIMER_GET_TIMER_ID(timer);
  OS_TASK_NOTIFY(task, COUNTER_NOTIF, eSetBits);
}

