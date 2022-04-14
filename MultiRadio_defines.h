/*
 * MultiRadio_defines.h
 *
 *  Created on: 13 Apr 2018
 *      Author: TP
 */


#ifndef MULTIRADIODEFINES_H
#define MULTIRADIODEFINES_H

/* PMSIS includes. */
#include "pmsis.h"

// pin used as GPIO
#define SPI2_DATAREADY_PI  PI_GPIO_A5_PAD_17_B40    /*!< GPIO_A5 */
#define SPI_BUSY_PI PI_GPIO_A19_PAD_33_B12 /*!< GPIO_A19 */

#define SPI2_PWR_PI PI_GPIO_A3_PAD_15_B1      /*!< GPIO_A3 */
#define SPI2_RST_PI PI_GPIO_A2_PAD_14_A2     /*!< GPIO_A2 */


//#define PIR_DETECTION_GPIO 0
//#define PIR_DETECTION_PAD 4

#define __UNUSED__ 0



#define REG(x)	  *(volatile unsigned int*)(x)	


void _Error_Handler(char *file, int line);
void memcpy_int(uint32_t *str1, const uint32_t *str2, size_t n);


// -----------------------------------------------
// configs
#define TIMEOUT_OP		      	(3000)
#define TMEOUT_CON		      	(3000)
#define TMEOUT_IMG_TX			(500)


#define ENABLE_BLT    	(0)
#define ENABLE_LORA		(0)

/* Spi clock in Hz */
#define SPI_BAUDRATE  (20000000)

// channel used for SPI to shield
#define SPI_CHANNEL (1)  

#define UDP_PORT 		  (8888)
#define UDP_SERVER		"155.185.49.29"

#define UDP_SCK 		  (1)

#define DATA_PAY_S		(500)  //LORA_PAY_S  [quindi pacchetti di 


#define WIFI_SSID     "VANGOGH"
#define WIFI_PASS     "vangogh123"
// ---------------------------------------------------------------------------------





#endif
