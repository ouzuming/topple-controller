/*
 * lowPower.h
 *
 *  Created on: 2017Äê6ÔÂ1ÈÕ
 *      Author: Administrator
 */

#ifndef LOWPOWER_INC_LOWPOWER_H_
#define LOWPOWER_INC_LOWPOWER_H_

#include "AHRS_alg.h"
#include <osal.h>

/**
 * enter low power mode time definition
 */

#define ENTER_LOWPOWER_TIME                                 100000


#define COUNTER_NOTIF                                       (1<<1)
#define POWEROFF_TIME_COUNTER_NOTIF                         (1<<2)
#define CCC_NOTIFICATIONS_NOTIF                             (1<<3)
#define COUNTER_TEME_START_NOTIF                            (1<<6)
#define COUNTER_TEME_STOP_NOTIF                             (1<<7)
#define POWER_OFF_COMMAND_NOTIF                             (1<<8)

/*define in task_config*/
//#define SUTOA_START_NOTIF                                   (1<<4)
//#define SUTOA_END_NOTIF                                     (1<<5)

/**
 * Function declaration
 */
void power_task(void *params);
void counter_time_cb( OS_TIMER timer);

#endif /* LOWPOWER_INC_LOWPOWER_H_ */
