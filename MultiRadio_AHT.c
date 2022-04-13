/*
 * MultiRadio_AHT.c
 *
 *  Functions to be called by the AHT main
 */



#include "MultiRadio_AHT.h"
#include "img2jpg.h"

#include "aht_defines.h"


// buffer ricezione e trasmissione; 
uint8_t *tx_buff;
uint8_t *rx_buff;

uint8_t ackok_flag = FALSE;
uint8_t ackno_flag = FALSE;
uint8_t wtnup_flag = FALSE;


uint8_t tx_flag = FALSE;
uint8_t half_buffer=0;


static AppWifi_e stato = APPWIFI_RESET;
//static Transfer_e wTransferState = TRANSFER_WAIT;
static tximg_e tx_img_stato;



uint8_t rx_complete =0;
uint8_t rx_start =0;


unsigned int time_execute = 0;

uint32_t img_number = 0;
uint16_t count_img = 0;


void initMultiradio(){
	MX_SPI_Init();
	MX_GPIO_Init();
	InitBuffers();
	}


int initWiFi(){
    
    uint16_t tx_size;

    while (stato != APPWFI_TXRX)
     {
	switch (stato){
		case APPWIFI_RESET:
			// reset wifi
			printf("MultiRadio_Init  \n\r" );
			MultiRadio_Init();
			

			tx_size = Gen_WPWRC(tx_buff, 1);

			SPI_ExtInterface ( tx_buff, rx_buff, tx_size);

			Start_Timer(TIMEOUT_OP);

			//printf("Command: WPWRC  \n\r" );

			stato = APPWIFI_POWERCONFIG;

			break;

		case APPWIFI_POWERCONFIG:
			//printf("APPWIFI_POWERCONFIG  -" );
			if(ackok_flag){
				printd("ackok_flag true  \n\r" );
				ackok_flag = FALSE;

				tx_size = Gen_WSSID(tx_buff,WIFI_SSID, WIFI_PASS, "MRadio", TRUE,
						0, 0, 0, 0, 0,SSID_SECURITY_WPA2);

				SPI_ExtInterface ( tx_buff, rx_buff, tx_size);

				printd("Command: WSSID  \n\r" );

				stato = APPWIFI_SSID;
			}
			else
				printd("ackok_flag false  \n\r" );
				
			if (ackno_flag){
				ackno_flag = FALSE;
			}

			if (Is_Timer_Elapsed ()){
				stato = APPWIFI_RESET;
			}
			break;

		case APPWIFI_SSID:
			//printf("APPWIFI_SSID  \n\r" );
			if((ackok_flag==TRUE) && (wtnup_flag==TRUE)){
				ackok_flag = FALSE;
				uint32_t iphex;
				iphex = ip_to_int (UDP_SERVER);
				tx_size = Gen_WSOCK(tx_buff, iphex, UDP_PORT, UDP_SCK, SCK_UDP, TRUE);
				SPI_ExtInterface ( tx_buff, rx_buff, tx_size);

				stato = APPWIFI_SCK;
				printf("WIFI YEAH!\n\r" );


			}
			if (ackno_flag){
				ackno_flag = FALSE;
				printf("Error Handler\n\r" );
				//_Error_Handler(__FILE__, __LINE__);
			}
			break;


		case APPWIFI_SCK:
			//printf("APPWIFI_SCK  \n\r" );
			//stato = APPWFI_TXRX;
			if(ackok_flag){
				ackok_flag = FALSE;

				printf("Starting  TX----------\n\r" );

				stato = APPWFI_TXRX;
			}
			if (ackno_flag){
				ackno_flag = FALSE;
				printf("Error Handler\n\r" );
			}
			break;

		
		} // end switch 


		/*wait answer*/
	
		if(Is_DataReady()){
			/* receive new data and execute callback */
			printf("Event: DataReadyPINClbk  \n\r" );
			SPI_ExtInterface ( tx_buff, rx_buff, FALSE);
		}


	} //while stato

  printf("wifi initialization completed !!!!!!!! \n");
return 0;
}


int stopWifi(){

  // deallocate buffers
  pmsis_malloc_free(rx_buff); 
  pmsis_malloc_free(tx_buff); 
  rx_buff= NULL;
  tx_buff=NULL;
  
 
 
	
}


int WIFI_sendData(const unsigned char * dataToSend, uint32_t size, uint8_t headerType, struct pi_device *pdeviceL3){


	static uint8_t *pdata;

	if (stato != APPWFI_TXRX)
		{
		printf("cannot send data \n");
		}

	tx_flag=TRUE;

	printf("sending data of type %d \n",headerType);
	
	
	 //RV TODO
	/*	// JPEG ENCODER
		{
		printf("encoding image \n");
		pdata = Img2jpg( dataToSend  , &size);
		}
	else
		{*/
		pdata = dataToSend; 
		printf("data size: %d\n", size);
		//RV}
		


	/* ask for send */
	tx_img_stato = TX_IMG_INIT;
	stato = APPWFI_TXDT;
	Start_Timer(TMEOUT_CON);
	
	ackok_flag = FALSE;
	
	printd("network status: %d\n",wtnup_flag);
	

	while (wtnup_flag)
	{
		
		/* discard images too big: why?!?!*/
		if(size < 0xFFFFFF)
		{
			/* send many pkts */
			if(pdata!=0)
			{	
			     Send_Data_Raw(pdata,size, headerType,pdeviceL3);
			}
			
			if (tx_img_stato == TI_IMG_END){
				/* end of send process */
				Start_Timer(TMEOUT_IMG_TX);
				stato = APPWFI_TXRX;
				printf(" ------- >     IMAGE SENT!!!!!   <------------------\n");
				break;
			}
		}
		else{
			printf("image too big...\n");	
			stato = APPWFI_TXRX;
			break;

		}
		if(ackok_flag)
		{
			ackok_flag = FALSE;
			DELAY_MS(1);
			printf("Image sent!\n\r" );
		}
		if (ackno_flag)
		{
			ackno_flag = FALSE;
			printf("Image not sent!\n\r" );
			DELAY_MS(1);
		}

		/*wait answer*/
	
		if(Is_DataReady()){
			/* receive new data and execute callback */
			printd("Event: DataReadyPINClbk  \n\r" );
			SPI_ExtInterface ( tx_buff, rx_buff, FALSE);
		}

		

	} // end while forever

	//ImgFree  (img);

	return 0;
}



void InitBuffers(){
	// 


	//rx_buff = rt_alloc(RT_ALLOC_PERIPH, SPI_BUFF_DIM);
	rx_buff = (uint8_t *) pmsis_l2_malloc((uint32_t) SPI_BUFF_DIM);
	
	if (rx_buff == NULL) return;
	//tx_buff = rt_alloc(RT_ALLOC_PERIPH, SPI_BUFF_DIM);
	tx_buff = (uint8_t *) pmsis_l2_malloc((uint32_t) SPI_BUFF_DIM);
	if (tx_buff == NULL) return;

	/* Init the buffer TODO: 10?!?! */
	for (int i=0; i<10; i++)
	{
		tx_buff[i] = i;
	}
	for (int j=0; j<10; j++)
	{
		rx_buff[j] = j;
	}

}

#ifdef USE_QSPI
struct pi_device QspiRam;
#define EXTERNAL_RAM QspiRam
#else
struct pi_device HyperRam;
#define EXTERNAL_RAM HyperRam
#endif
#ifdef USE_QSPI
# include "bsp/flash/spiflash.h"
# include "bsp/ram/spiram.h"
#else
# include "bsp/flash/hyperflash.h"
# include "bsp/ram/hyperram.h"
#endif


/**
 * @brief  Send Img Raw
 * @param  img: Img pointer
 * @param  size: Img size
 * @retval None
 */
void Send_Data_Raw(uint8_t *data, uint32_t size, uint8_t headerType, struct pi_device *pdeviceL3){


	static uint32_t res;
	uint16_t tx_size,c_img;
	uint8_t* udppay;

	switch(tx_img_stato){
	case TX_IMG_INIT:
		count_img = 0;
		res = size/DATA_PAY_S;  // res = numero pacchetti da inviare
		if ((size%DATA_PAY_S)>0){
			res += 1;
		}
		printf("Sending Img... S:%d\n\r",size);
		printf("N TX:%d\n\r",res);
		tx_img_stato = TX_IMG_SEND1;
		break;
		
	case TX_IMG_SEND1:
		if(count_img < res){
			ackok_flag = FALSE;
			ackno_flag = FALSE;

			/* last packet flag */   // c_img contiene il numero del pacchetto oppure 0xFFFF se siamo alla fine
			if ((count_img == (res-1))){
				c_img = 0xFFFF;
			}else{
				c_img = count_img;
			}
			
			// creo pacchetto dati: 
			// header con c_img, poi dati immagine  (DATA_PAY_S) 
			tx_size = Gen_WTXCM_Pointer( tx_buff, UDP_SCK, sizeof(c_img) +sizeof(headerType) + DATA_PAY_S , &udppay);
			
			memcpy(udppay,      &c_img,sizeof(c_img));
			memcpy(udppay+sizeof(c_img),  &headerType,sizeof(headerType));
			if (pdeviceL3)
				pi_ram_read(pdeviceL3, data+ (DATA_PAY_S*count_img), udppay+sizeof(c_img)+sizeof(headerType), (uint32_t) DATA_PAY_S);
			else
				memcpy(udppay+sizeof(c_img)+sizeof(headerType), data+ (DATA_PAY_S*count_img), DATA_PAY_S);

			SPI_ExtInterface ( tx_buff, rx_buff, tx_size);
			//RV  printf("Count Img: %d\n\r",count_img);
			//RV printf("TX Size: %d\n\r",tx_size);

			Start_Timer(TMEOUT_IMG_TX);


			tx_img_stato = TX_IMG_WAIT1;
		}else{
			printf("Count Img: END\n\r");
			tx_img_stato = TI_IMG_END;
		}
		break;

	case TX_IMG_WAIT1:

		ackok_flag = FALSE;
		ackno_flag = FALSE;
		count_img++;
		pi_time_wait_us(1000);
		tx_img_stato = TX_IMG_SEND1;

		break;
	case TX_IMG_WAIT2:
		if(Is_Timer_Elapsed()){
			tx_img_stato = TX_IMG_SEND1;
		}
		break;
	case TI_IMG_END:
		printf("Count Img: END\n\r");
		break;
	}

}


