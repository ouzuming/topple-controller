/*
 * fusion.c
 *
 *  Created on: 2017Äê7ÔÂ6ÈÕ
 *      Author: Administrator
 */
#include "fusion.h"
#include "notify.h"
#include "axis.h"
#include <osal.h>
#include "lowPower.h"
#include "key.h"
#include "ICM2060x.h"
#include "akm09915.h"
#include "AHRS_alg.h"
#include "algo_init.h"
#include "pxp_flash.h"

//#define FUSION_PRINTF
OS_QUEUE coeffQueue;
uint8_t g_package_head = 0;
uint8_t g_SN = 0;
uint8_t g_pcCommand = 0;
uint8_t g_commandLength = 0;
uint8_t g_subPackageFlag = 0;
uint8_t fusion_coeffBuf[CALIBRATION_DATA_LEN] = { 0 };
uint8_t packageBuf[150] = { 0 };
uint8_t g_package_point = 0;

uint8_t g_COM_A4_Flag = 0;
uint8_t g_FUSION_MODE_FLAG = 0;

void sps_rx_data_cb(ble_service_t *svc, uint16_t conn_idx, const uint8_t *value,
                    uint16_t length) {
  uint8_t buf[144] = { 0 };
  uint8_t bcc = 0;


  #ifdef FUSION_PRINTF
//  printf("sps_rx_data_cb \n");
//  //sps_tx_data(svc, conn_idx, value, length); // WILL NOTIFY WHAT VALUE HAS BEEN WRITTEN in the RX char.
//  printf("sps_rx->length= %d \n", length);
//  printf("sps_rx->value=");
//  for (uint8_t i = 0; i < length; i++) {
//    printf("0x%02x ", value[i]);
//  }
//  printf("\n");
  #endif

#if 1
  if (!g_subPackageFlag) {
    g_package_point = 0;
    g_package_head = value[HEAD_BIT] & 0x80;
    g_SN = value[HEAD_BIT] & 0x1f;
    g_commandLength = value[LEN_BIT];
    g_pcCommand = value[COMMAND_BIT];

    if (g_commandLength < PACKAGE_LEN) {
      bcc = 0;
      for (uint8_t i = 0; i < length - 1; i++) {
        bcc ^= value[i];
      }
      if (bcc != value[length - 1]) {
        #ifdef FUSION_PRINTF
        printf(" bcc1 error \n");
        #endif
        return;
      }
      memcpy(&packageBuf[g_package_point], value, length);

    } else {
      g_subPackageFlag = 1;
      memcpy(&packageBuf[g_package_point], value, length);
      g_package_point += length;
      return;
    }

  } else {
    memcpy(&packageBuf[g_package_point], value, length);
    g_package_point += length;



    if (g_package_point > g_commandLength) {             // reveive all the data
       #ifdef FUSION_PRINTF
       printf("rec package finish...... \n");
       #endif

#ifdef FUSION_PRINTF
      printf("packageBuf length= %d",g_package_point);
      for (uint8_t i = 0; i < g_package_point; i++) {
        printf("0x%02x ", packageBuf[i]);
      }
      printf("\n");
#endif

      g_subPackageFlag = 0;
      bcc = 0;
      for (uint8_t i = 0; i < g_package_point - 1; i++) {
        bcc ^= packageBuf[i];
      }
      if (bcc != packageBuf[g_package_point - 1]) {
        #ifdef FUSION_PRINTF
        printf(" bcc2 error \n");
        #endif
        return;
      }
    } else {
      #ifdef FUSION_PRINTF
      printf("wait next package...... \n");
      #endif
      return;
    }
  }

  if (g_package_head == PC_ST) {
    switch (g_pcCommand) {
      case COM_A1:
        #ifdef FUSION_PRINTF
        printf(" com_A1__\n");
        #endif
        g_COM_A4_Flag = 0;
        OS_TASK_NOTIFY(fusion_handle, COM_A1_NOTIF, eSetBits);
        break;

      case COM_A2:
        #ifdef FUSION_PRINTF
        printf(" com_A2__\n");
        #endif
        OS_TASK_NOTIFY(fusion_handle, COM_A2_NOTIF, eSetBits);
        break;

      case COM_A3:
        #ifdef FUSION_PRINTF
        printf(" com_A3__\n");
        #endif
        g_COM_A4_Flag = 0;
        OS_TASK_NOTIFY(fusion_handle, COM_A3_NOTIF, eSetBits);
        break;

      case COM_A4:
        #ifdef FUSION_PRINTF
        printf(" com_A4__\n");
        #endif
        g_COM_A4_Flag = 1;
        OS_TASK_NOTIFY(fusion_handle, COM_A4_NOTIF, eSetBits);
        break;

      case COM_A5:
        #ifdef FUSION_PRINTF
        printf(" com_A5__\n");
        #endif
        memcpy(fusion_coeffBuf, &packageBuf[COMEFF_BIT], g_commandLength);
        if ((uxQueueSpacesAvailable(coeffQueue)) >= 1) {
          OS_QUEUE_PUT(coeffQueue, fusion_coeffBuf, 0);
        } else {
          OS_QUEUE_GET(coeffQueue, buf, 0);
          OS_QUEUE_PUT(coeffQueue, fusion_coeffBuf, 0);
        }
        OS_TASK_NOTIFY(fusion_handle, COM_A5_NOTIF, eSetBits);
        break;

      case COM_A6:
        #ifdef FUSION_PRINTF
        printf(" com_A6__\n");
        #endif
        OS_TASK_NOTIFY(fusion_handle, COM_A6_NOTIF, eSetBits);
        break;

      case COM_A7:
        #ifdef FUSION_PRINTF
        printf(" com_A7__\n");
        #endif
        OS_TASK_NOTIFY(fusion_handle, COM_A7_NOTIF, eSetBits);
        break;

      case COM_A8:
        #ifdef FUSION_PRINTF
        printf(" com_A8__\n");
        #endif
        OS_TASK_NOTIFY(fusion_handle, COM_A8_NOTIF, eSetBits);
        break;

      default:
        #ifdef FUSION_PRINTF
        printf(" com_default__\n");
        #endif
        response_command(g_SN, RESPONSE_ERROR_1);
        break;
    }
  }else{
    #ifdef FUSION_PRINTF
    printf(" package_head error ");
    #endif
  }
#endif
}

//notify
//sps_tx_data  after
//callback  handle_event_sent
// callback   sps_tx_done_cb
void sps_tx_done_cb(ble_service_t *svc, uint16_t conn_idx, uint16_t length) {
//printf("sps_tx_done_cb \n");
// The notification is already sent out

}

void sps_set_flow_control_cb(ble_service_t *svc, uint16_t conn_idx,
                             sps_flow_control_t value) {
  printf("sps_set_flow_control_cb \n");
  sps_set_flow_control(svc, conn_idx, value);  // Enable the flow control
}

/**
 * @fn      LED_task
 * @brief   led task function
 * @param   none
 * @return  none
 */
void fusion_task(void *params) {
  static uint32_t sensorSecond = 0;
  uint8_t uploadTime = 0;
  uint8_t fpoint = 0;
  int16_t sensor_data[9] = { 0 };
  uint8_t fusionBuf[20] = { 0 };
  uint8_t coeffBuf[CALIBRATION_DATA_LEN+1] = { 0 };
  SHORT_UNION short_union;
  uint16_t mag_counter = 0;

  /* create queue */
  OS_QUEUE_CREATE(coeffQueue, sizeof(fusion_coeffBuf), 1);
  OS_ASSERT(coeffQueue);

  OS_BASE_TYPE ret;
  uint32_t notif = 0;

  PRIVILEGED_DATA static OS_TIMER *sensor_timer;
  sensor_timer = OS_TIMER_CREATE("sensor_timer", OS_MS_2_TICKS(3),  //2
                                 OS_TIMER_SUCCESS,
                                 (void *) OS_GET_CURRENT_TASK(), fusion_cb);
  OS_ASSERT(sensor_timer);
  // OS_TIMER_START(sensor_timer, OS_TIMER_FOREVER);
  OS_DELAY_MS(50);
  for (;;) {
    ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif,
                              OS_TASK_NOTIFY_FOREVER);
    OS_ASSERT(ret == OS_OK);

    /* upload data mode  */
    if (notif & FUSION_NOTIF) {

      #ifdef FUSION_PRINTF
      printf("notif & FUSION_NOTIF\n");
      #endif
      OS_TIMER_STOP(sensor_timer, OS_TIMER_FOREVER);
      sensorSecond = OS_GET_TICK_COUNT() * 2;               // ms
      uploadTime = sensorSecond & 0x000000ff;

      if ( READ_AK_DRDY) {
        mag_counter = 0;
        magnet_get_data(&sensor_data[6]);
        #ifdef FUSION_PRINTF
        printf(" akm_x =%d, akmy_y =%d,akm_z =%d, \n\r", sensor_data[6],sensor_data[7], sensor_data[8]);
        #endif
      }
      else {
         mag_counter++;
        if( mag_counter > 2 ){
                for (uint8_t ibit = 0; ibit < 3; ibit++) {
                  sensor_data[ibit + 6] = 0x00;
                }
        }
        if( mag_counter > 100 ){
            akm09915_fusion_init();
        }
      }
      if (READ_ICM_INT) {
        fpoint = 0;
        fusionBuf[fpoint++] = HEAD_STN_B1;
        ICM2060x_GetACC(&sensor_data[0]);
        for (uint8_t ibit = 0; ibit < 3; ibit++) {
          short_union.value = sensor_data[ibit];
          fusionBuf[fpoint++] = short_union.short_byte.high_byte;
          fusionBuf[fpoint++] = short_union.short_byte.low_byte;
        }
        ICM2060x_GetGYRO(&sensor_data[3]);
        for (uint8_t ibit = 0; ibit < 3; ibit++) {
          short_union.value = sensor_data[3 + ibit];
          fusionBuf[fpoint++] = short_union.short_byte.high_byte;
          fusionBuf[fpoint++] = short_union.short_byte.low_byte;
        }

        #ifdef FUSION_PRINTF
        printf(" accel_x =%d, accel_y =%d,accel_z =%d, \n\r", sensor_data[0], sensor_data[1], sensor_data[2]);
        printf(" gyro_x =%d, gyro_y =%d,gyro_z =%d, \n\r", sensor_data[3], sensor_data[4], sensor_data[5]);
        #endif

        for (uint8_t ibit = 0; ibit < 3; ibit++) {
          short_union.value = sensor_data[6 + ibit];
          fusionBuf[fpoint++] = short_union.short_byte.high_byte;
          fusionBuf[fpoint++] = short_union.short_byte.low_byte;
        }
        fusionBuf[fpoint++] = uploadTime;
        ble_sendData(fusionBuf);
      }
      OS_TIMER_START(sensor_timer, OS_TIMER_FOREVER);
    }

    /* enter fusion mode  */
    if (notif & COM_A1_NOTIF) {
       #ifdef FUSION_PRINTF
       printf("notif & COM_A1_NOTIF\n");
       #endif
       OS_TASK_NOTIFY(led_handle, START_FUSION_TIMER_NOTIF, eSetBits);
       g_FUSION_MODE_FLAG = 1;
       cm_sys_clk_set(sysclk_PLL96);
       OS_TASK_NOTIFY(notify_handle, STOP_TIME_NOTIF, eSetBits);
       OS_TASK_NOTIFY(axis_handle, AXIS_TIMER_STOP_NOTIF, eSetBits);
       OS_TASK_NOTIFY(power_handle, COUNTER_TEME_STOP_NOTIF, eSetBits);
       OS_DELAY_MS(200);        // delay to let other task to run stop
        /* init sensor */
        akm09915_fusion_init();
        ICM2060x_Init();
        OS_DELAY_MS(10);
        OS_TIMER_START(sensor_timer, OS_TIMER_FOREVER);
    }

    /* quit fusion mode  */
    if (notif & COM_A2_NOTIF) {
      #ifdef FUSION_PRINTF
      printf("notif & COM_A2_NOTIF\n");
      #endif
      g_FUSION_MODE_FLAG = 0;
      cm_sys_clk_set(sysclk_PLL48);
      OS_TASK_NOTIFY(led_handle, STOP_FUSION_TIMER_NOTIF, eSetBits);
    }

    /* request upload data */
    if (notif & COM_A3_NOTIF) {
      #ifdef FUSION_PRINTF
      printf("notif & COM_A3_NOTIF\n");
      #endif
      akm09915_fusion_init();
      ICM2060x_Init();
      OS_DELAY_MS(10);
      OS_TIMER_START(sensor_timer, OS_TIMER_FOREVER);
    }

    /* request stop upload data */
    if (notif & COM_A4_NOTIF) {
      #ifdef FUSION_PRINTF
      printf("notif & COM_A4_NOTIF\n");
      #endif
      OS_TIMER_STOP(sensor_timer, OS_TIMER_FOREVER);
      OS_DELAY_MS(5);
      response_command(g_SN, g_pcCommand);
    }

    /* issued calibration data */
    if (notif & COM_A5_NOTIF) {

      #ifdef FUSION_PRINTF
      printf("notif & COM_A5_NOTIF\n");
      #endif

      if ( OS_QUEUE_GET(coeffQueue, &coeffBuf[1], OS_QUEUE_NO_WAIT) == OS_QUEUE_OK) {
        // write flash:
      #ifdef FUSION_PRINTF
      printf("com_A5 write data 32bytes: ");
      for(uint8_t i = 0; i< 32; i++){
        printf("0x%02x ", coeffBuf[i]);
      }
      printf("\n");
      #endif

        coeffBuf[0] = WRITE_CALIBRATION_FLAG;
        write_flash_data(coeffBuf, 0, CALIBRATION_DATA_LEN+1);
        response_command(g_SN, g_pcCommand);
      }else {
        #ifdef FUSION_PRINTF
        printf("get coeffqueue fail : \n ");
        #endif
        response_command(g_SN, RESPONSE_ERROR_1);
      }
    }

    /* read controler flash calibration data */
    if (notif & COM_A6_NOTIF) {
      #ifdef FUSION_PRINTF
      printf("notif & COM_A6_NOTIF\n");
      #endif

      read_flash_data(coeffBuf, 0, CALIBRATION_DATA_LEN+1);

      #ifdef FUSION_PRINTF
      printf("com_A6 read data 32bytes: ");
      for(uint8_t i = 0; i< 32; i++){
        printf("0x%02x ", coeffBuf[i]);
      }
      printf("\n");
      #endif

      if( coeffBuf[0] == WRITE_CALIBRATION_FLAG ){
        response_calibration_data( g_SN, &coeffBuf[1], CALIBRATION_DATA_LEN );
      }else{
        response_command(g_SN, RESPONSE_ERROR_1);
      }
    }
    /* control controler power off  */
    if (notif & COM_A7_NOTIF) {

      #ifdef FUSION_PRINTF
      printf("notif & COM_A7_NOTIF\n");
      #endif
      response_command(g_SN, g_pcCommand);
      OS_DELAY_MS(5);
      OS_TASK_NOTIFY(power_handle, POWER_OFF_COMMAND_NOTIF, eSetBits);
    }

    /* check calibration status  */
    if (notif & COM_A8_NOTIF) {

      #ifdef FUSION_PRINTF
      printf("notif & COM_A8_NOTIF\n");
      #endif
      read_flash_data(coeffBuf, 0, 1);
      if( coeffBuf[0] == WRITE_CALIBRATION_FLAG ){
        response_command(g_SN, g_pcCommand);
      }else{
        response_command(g_SN, RESPONSE_ERROR_1);
      }
#ifdef FUSION_PRINTF
     for(uint8_t i = 0; i < 12; i++){
         printf("acc_coeff[%d] = %d \n", i,(int)(acc_coeff[i]*100000000) );
     }
     for(uint8_t i = 0; i < 12; i++){
         printf("gyro_coeff[%d] = %d \n",i, (int)(gyro_coeff[i]*100000000) );
     }
     for(uint8_t i = 0; i < 12; i++){
         printf("mag_coeff[%d] = %d \n", i,(int)(mag_coeff[i]*100000000) );
     }
#endif
    }
  }
}

/**
 * @brief   initialize led pin
 * @param   none
 * @return  none
 */
void fusion_cb( OS_TIMER timer) {
  if( !g_COM_A4_Flag ){
      OS_TASK task = (OS_TASK) OS_TIMER_GET_TIMER_ID(timer);
      OS_TASK_NOTIFY(task, FUSION_NOTIF, eSetBits);
  }
}

/**
 * @brief   response pc command
 * @param   snValue: pc sn number
 *          commandValue: pc command
 * @return  none
 */
void response_command(uint8_t snValue, uint8_t commandValue) {
  uint8_t resBuf[5] = { 0 };
  uint8_t resPoint = 0;
  uint8_t bcc = 0;

  resBuf[resPoint++] = CONTROLER_ST | snValue;
  resBuf[resPoint++] = 0x01;
  resBuf[resPoint++] = CONTROLER_RESPONSE_COMMAND;
  resBuf[resPoint++] = commandValue;
  for (uint8_t ibit = 0; ibit < resPoint; ibit++) {
    bcc ^= resBuf[ibit];
  }
  resBuf[resPoint++] = bcc;
  ble_sendDataBuf(resBuf, resPoint);
}

/**
 * @brief   response pc command
 * @param   snValue: pc sn number
 *          commandValue: pc command
 * @return  none
 */
void response_calibration_data(uint8_t snValue, uint8_t *calibrationData, uint8_t calibrationLength ) {
  uint8_t resBuf[148] = { 0 };
  uint8_t resPoint = 0;
  uint8_t bcc = 0;

  resBuf[resPoint++] = CONTROLER_ST | snValue;
  resBuf[resPoint++] = 0x91;
  resBuf[resPoint++] = CONTROLER_CALIBRATION_COMMAND;
  memcpy(&resBuf[resPoint], calibrationData, calibrationLength );
  resPoint += calibrationLength;
  for (uint8_t ibit = 0; ibit < resPoint; ibit++) {
    bcc ^= resBuf[ibit];
  }
  resBuf[resPoint++] = bcc;
  ble_sendDataBuf(resBuf, resPoint);

}

