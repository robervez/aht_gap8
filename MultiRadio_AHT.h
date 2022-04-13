/*
 * MultiRadio_AHT.h
 *
 *  Functions to be called by the AHT main
 */


#ifndef MULTIRADIO_AHT_H
#define MULTIRADIO_AHT_H

#include "MultiRadio_defines.h"
#include "MultiRadio_library.h"


typedef enum{
	APPWIFI_RESET,
	APPWIFI_POWERCONFIG,
	APPWIFI_SSID,
	APPWIFI_SCK,
	APPWFI_TXRX,
	APPWFI_TXDT,
	APP_SLEEP
}AppWifi_e;

/*enum {
	TRANSFER_WAIT,
	TRANSFER_COMPLETE,
	TRANSFER_ERROR
} Transfer_e;*/

typedef enum{
	TX_IMG_INIT,
	TX_IMG_SEND1,
	//TX_IMG_SEND2,
	TX_IMG_WAIT1,
	TX_IMG_WAIT2,
	TI_IMG_END
}tximg_e;





/* Private function prototypes -----------------------------------------------*/

void InitBuffers();
void Send_Data_Raw(uint8_t *img, uint32_t size, uint8_t headerType, struct pi_device *pdeviceL3);


/* Private function prototypes -----------------------------------------------*/


/* Public function prototypes -----------------------------------------------*/

void initMultiradio();
	
	
int initWiFi();
int stopWifi();

int WIFI_sendData(const unsigned char * dataToSend, uint32_t size, uint8_t headerType, struct pi_device *pdeviceL3);

/* Public function prototypes -----------------------------------------------*/


#endif



