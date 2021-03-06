/*
 * MultiRadio_library.c
 *
 *  Created on: 13 Apr 2018
 *      Author: TP
 */


#include "string.h"
#include "stdio.h"

#include "pmsis.h"

#include "MultiRadio_library.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define SET_TX_SIZE(x,size)			do{x[SPI_LEN_POS]=(uint8_t)((size >> 8) & 0xFF);    \
										x[SPI_LEN_POS+1]=(uint8_t)(size & 0xFF);		\
									}while(0)
#define GET_RX_SIZE(x)				((((uint16_t)x[SPI_LEN_POS]) << 8) | ((uint16_t)x[SPI_LEN_POS+1]))
#define QUEUE_IP_BYTES(buf,pos,ip)	do{													\
									buf[pos]   = (uint8_t)((ip >> 24) & 0xFF);			\
									buf[pos+1] = (uint8_t)((ip >> 16) & 0xFF);			\
									buf[pos+2] = (uint8_t)((ip >> 8 ) & 0xFF);			\
									buf[pos+3] = (uint8_t)((ip      ) & 0xFF);			\
									}while(0)
#define QUEUE_PORT_BYTES(buf,pos,ip)	do{												\
									buf[pos  ] = (uint8_t)((ip >> 8 ) & 0xFF);			\
									buf[pos+1] = (uint8_t)((ip      ) & 0xFF);			\
									}while(0)
#define QUEUE_DATA_SIZE(buf,pos,ip)	do{													\
									buf[pos  ] = 0;										\
									buf[pos+1] = 0;										\
									buf[pos+2] = (uint8_t)((ip >> 8 ) & 0xFF);			\
									buf[pos+3] = (uint8_t)((ip      ) & 0xFF);			\
									}while(0)

/* Must be enabled if the SPI_TRANSFER(tx,rx,len) supports the full-duplex communication */
#define SPI_FULL_DUPLEX				(1)


/* Transfer SPI function */
#define SPI_TRANSFER(tx,rx,len,cs)	do{pi_spi_transfer(&spim,tx,rx,(len) * 8,cs);}while(0)
#define SPI_RECEIVE(rx,len,cs)	do{pi_spi_receive(&spim,rx,(len) * 8,cs);}while(0)
#define SPI_TRANSMIT(tx,len,cs)	do{pi_spi_send(&spim,tx,(len) * 8,cs);}while(0)



#define SPI_SELECT()				((void)0)
#define SPI_UNSELECT()				((void)0)
#define IS_MASTER_SPI_BUSY()		FALSE

	

/* Power PIN */
#define MOD_RESET()					do{pi_gpio_pin_write(&gpio, SPI2_RST_PI, 0);}while(0)
#define MOD_RELEASE()				do{pi_gpio_pin_write(&gpio, SPI2_RST_PI, 1);/*rt_gpio_set_dir(0, 1<<SPI2_RST_PIN, RT_GPIO_IS_IN);*/}while(0)
#define POWER_UP()					pi_gpio_pin_write(&gpio, SPI2_PWR_PI, 1)
#define POWER_DWN()					pi_gpio_pin_write(&gpio, SPI2_PWR_PI, 0)
#define POWER_WAIT_ON()			    pi_time_wait_us(10000);

//



/* For RTOS */

#define SPI_WAIT_SLEEP()			((void)0)//pi_time_wait_us (100)

/* Error management */
#define SPI_EXE_ERROR()				do{_Error_Handler(__FILE__, __LINE__);}while(0)

/* Function to get the error in SPI driver */
#define SPI_GET_ERR()				((void)0)

/* High speed memcopy */
#define SPI_HS_M2M(src,dest,len)	do{memcpy(dest,src,len);}while(0)

#define SPI_TIMEOUT					(10000)



/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

//extern void rt_spim_transfer(struct pi_device *device, void *tx_data, void *rx_data, size_t len, rt_spim_cs_e cs_mode, rt_event_t *event);
//RV PERCHE? extern void pi_spi_transfer(struct pi_device *device, void *tx_data, void *rx_data,  size_t len, pi_spi_flags_e flag);


/******************************************************
 *               Variable Definitions
 ******************************************************/

static uint8_t	rx_dataready = FALSE;


extern unsigned int time_execute;

// handlers
struct pi_device gpio;
/* spi handlers */
struct pi_device spim = {0};

static uint32_t wait_timer;



/******************************************************
 *               Function Definitions
 ******************************************************/

inline uint8_t IS_SLAVE_SPI_BUSY(){
	uint32_t ris;
	pi_gpio_pin_read( &gpio,SPI_BUSY_PI, &ris);
	return (ris==TRUE);
}
		
inline uint8_t IS_SLAVE_DATA_READY()	{
	uint32_t ris;
	pi_gpio_pin_read( &gpio,SPI2_DATAREADY_PI, &ris);
	return (ris==TRUE);
}



const Api_callback_t Api_callback_Array[API_ARRAY_SIZE(ENUM_API_SIZE)] = {
		CMD_TABLE(EXPAND_AS_API_INIT_STR)
		{NULL,"NULL",FALSE}
};

const uint8_t API_CMD_STR_SIZE = API_ARRAY_SIZE(ENUM_API_SIZE);

CMD_TABLE(EXPAND_AS_API_WEAK_FUN)


/* INSERT HERE -----------------------------------------------------------------*/
/* INSERT HERE -----------------------------------------------------------------*/
/* INSERT HERE -----------------------------------------------------------------*/
/* INSERT HERE -----------------------------------------------------------------*/
/* INSERT HERE -----------------------------------------------------------------*/
/* INSERT HERE -----------------------------------------------------------------*/


/**
  * @brief  Full Duplex Transfer Data
  * @param  tx: pointer to a tx buffer, rx: pointer to rx buffer,
  * tx_size: tx payload lenght (command and data)
  * @retval rx_size: rx payload received (command and data)
  */
uint16_t SPI_ExtInterface ( uint8_t* tx , uint8_t* rx, uint16_t tx_size ){
	uint16_t rx_data_size = FALSE;
	uint16_t tx_data_size = FALSE;
	uint8_t* rx_p = rx;
	

	/* parameter not correct */
	if(tx_size > (PACKET_MAX_SIZE_WIFIAPP + API_CMD_LEN)){
		printd("A\r\n");
		return rx_data_size;
	}

	/* reset TX len */
	if(tx_size == FALSE){
		printd("B\r\n");
		memset(tx,FALSE,SPI_PAYLOAD_POS);
	}


	/* wait SPI ready */
	while(IS_MASTER_SPI_BUSY()){
		SPI_WAIT_SLEEP();
	}


	/* Start SPI */
	while(IS_SLAVE_SPI_BUSY()){
		SPI_WAIT_SLEEP();
	}


	SPI_SELECT();	
	

	if((rx_dataready == TRUE) || IS_SLAVE_DATA_READY()){

		rx_dataready = FALSE;

		/* Get the RX length */
		SPI_TRANSFER(tx, rx, PACKET_MAX_SIZE_WIFIAPP - 2, PI_SPI_CS_AUTO); 

		/* wait SPI ready */
		pi_time_wait_us (200);
		
		while(IS_MASTER_SPI_BUSY()){
			SPI_WAIT_SLEEP();
		}

		printd("SPRXRAW: %d_%d_%d\n\r",rx[0], rx[1], rx[2] );

		/* check RX length */
		rx++;
		rx_data_size = GET_RX_SIZE(rx);
		printd("DATASIZE: %d \n\r", rx_data_size);

		if (rx_data_size >= PACKET_MAX_SIZE_WIFIAPP){
			rx_data_size = FALSE;
		}
		/* transfer payload */
		rx = rx + SPI_PAYLOAD_POS;

		printd("DATASIZE: %d \n\r", rx_data_size);

		printd("SPRXRAW: %d_%d_%d_%c_%c_%c_%c_%c\n\r",			rx_p[0], rx_p[1], rx_p[2], rx_p[3], rx_p[4], rx_p[5], rx_p[6], rx_p[7] );

		printd("SPRXRAW: %c_%c_%c_%c_%c\n\r",			rx[0], rx[1], rx[2], rx[3], rx[4] );


	}else{


		tx_data_size = tx_size + SPI_PAYLOAD_POS;
		tx_data_size = tx_data_size + ((sizeof(uint32_t) - (tx_data_size & 0x03)));
		/* only TX */
		SPI_TRANSFER(tx , rx, tx_data_size , PI_SPI_CS_AUTO);
	}

	SPI_UNSELECT();

	if (rx_data_size > FALSE){
		/* execute command callback */
		ExecuteCommandClbk( rx, rx_data_size);
	}


	return rx_data_size;

}




/**
  * @brief  Decode the received command and execute associated callback
  * @param  rx: pointer to a rx buffer,
  * rx_data_size: rx payload lenght (command and data)
  * @retval None
  */
void ExecuteCommandClbk(uint8_t* rx, int16_t rx_data_size){
	int a;

	/* Search command */
    for (a = 0; a < API_CMD_STR_SIZE; a++){
        if (memcmp(Api_callback_Array[a].cmd,rx,API_CMD_LEN) == 0){
            /* received data and function call */
           	Api_callback_Array[a].Api_Callback(&rx[API_CMD_LEN]);
           	break;
        }
    }
}


/**
  * @brief  Wake up sequence (reset sequence)
  * @param  void
  * @retval void
  */
void MultiRadio_Init ( void ){
	/* di test */
	POWER_UP();
	MOD_RESET();
	POWER_WAIT_ON();
	POWER_WAIT_ON();
	MOD_RELEASE();

	/* wait 100 ms */
	for(int i=0;i<10;i++)POWER_WAIT_ON();

}

/**
  * @brief  Make the WPWRC data buffer
  * @param  buff: pointer to TX buffer, pwrmode: power setting
  * @retval buffer_size
  */
uint16_t Gen_WPWRC(uint8_t* buff, AppWiFi_LowPowerMode_e pwrmode){

	SET_TX_SIZE(buff,(Api_callback_Array[ENUM_WPWRC].cmd_len + API_CMD_LEN));
	memcpy(&buff[SPI_PAYLOAD_POS],Api_callback_Array[ENUM_WPWRC].cmd,API_CMD_LEN);
	buff[SPI_PAYLOAD_POS+API_CMD_LEN + WSSID_PWR_POS] = (uint8_t)pwrmode;

	return Api_callback_Array[ENUM_WPWRC].cmd_len + API_CMD_LEN;

}


/**
  * @brief  Make the WSSID data buffer
  * @param  buff: pointer to TX buffer
  * @retval buffer_size
  */
uint16_t Gen_WSSID(uint8_t* buff, char* AppWiFiName, char* AppWiFiPassword, char* DeviceName, uint8_t DHCP,
		uint32_t IP, uint32_t subnetmask, uint32_t DefaultGateway, uint32_t DNS1, uint32_t DNS2,
		AppWiFi_SSID_security_e security){

	/* check input values */
	if ((strlen(AppWiFiName) >= WSSID_APN_LEN) || (strlen(AppWiFiPassword) >= WSSID_APP_LEN)
			|| (strlen(DeviceName) >= WSSID_DNA_LEN)){
		/* input string wrong */
		SPI_EXE_ERROR();
	}
	if (!WSSID_DHCP_ASSERT(DHCP)){
		/* dhcp command wrong */
		SPI_EXE_ERROR();
	}

	SET_TX_SIZE(buff,(Api_callback_Array[ENUM_WSSID].cmd_len + API_CMD_LEN));

	memcpy(&buff[SPI_PAYLOAD_POS],Api_callback_Array[ENUM_WSSID].cmd,API_CMD_LEN);
	memcpy(&buff[SPI_PAYLOAD_POS + API_CMD_LEN + WSSID_APN_POS],AppWiFiName,strlen(AppWiFiName)+1);
	memcpy(&buff[SPI_PAYLOAD_POS + API_CMD_LEN + WSSID_APP_POS],AppWiFiPassword,strlen(AppWiFiPassword)+1);
	memcpy(&buff[SPI_PAYLOAD_POS + API_CMD_LEN + WSSID_DNA_POS],DeviceName,strlen(DeviceName)+1);
	buff[SPI_PAYLOAD_POS + API_CMD_LEN + WSSID_DHC_POS] = DHCP;
	QUEUE_IP_BYTES(buff,SPI_PAYLOAD_POS + API_CMD_LEN + WSSID_IPM_POS,IP);
	QUEUE_IP_BYTES(buff,SPI_PAYLOAD_POS + API_CMD_LEN + WSSID_SBM_POS,subnetmask);
	QUEUE_IP_BYTES(buff,SPI_PAYLOAD_POS + API_CMD_LEN + WSSID_GAT_POS,DefaultGateway);
	QUEUE_IP_BYTES(buff,SPI_PAYLOAD_POS + API_CMD_LEN + WSSID_DN1_POS,DNS1);
	QUEUE_IP_BYTES(buff,SPI_PAYLOAD_POS + API_CMD_LEN + WSSID_DN2_POS,DNS2);
	buff[SPI_PAYLOAD_POS + API_CMD_LEN + WSSID_SEC_POS] = (uint8_t)security;

	return Api_callback_Array[ENUM_WSSID].cmd_len + API_CMD_LEN;
}

/**
  * @brief  Make the WSSIU data buffer
  * @param  buff: pointer to TX buffer
  * @retval buffer_size
  */
uint16_t Gen_WSSIU( uint8_t* buff ){

	SET_TX_SIZE(buff,(Api_callback_Array[ENUM_WSSIU].cmd_len + API_CMD_LEN));
	memcpy(&buff[SPI_PAYLOAD_POS],Api_callback_Array[ENUM_WSSIU].cmd,API_CMD_LEN);

	return Api_callback_Array[ENUM_WSSIU].cmd_len + API_CMD_LEN;
}

/**
  * @brief  Make the WSOCK data buffer
  * @param  buff: pointer to TX buffer
  * @retval buffer_size
  */
uint16_t Gen_WSOCK( uint8_t* buff, uint32_t ServerIP, uint16_t ServerPort,
		uint8_t socketNumber, Socket_flag_e Socket_flag, uint8_t conf){


	if (!WSOCK_SKN_ASSERT(socketNumber)){
		/* socketNumber command wrong */
		SPI_EXE_ERROR();
	}
	if (!WSOCK_TIP_ASSERT(Socket_flag)){
		/* Socket_flag command wrong */
		SPI_EXE_ERROR();
	}
	if (!WSOCK_CON_ASSERT(conf)){
		/* conf command wrong */
		SPI_EXE_ERROR();
	}
	SET_TX_SIZE(buff,(Api_callback_Array[ENUM_WSOCK].cmd_len + API_CMD_LEN));

	memcpy(&buff[SPI_PAYLOAD_POS],Api_callback_Array[ENUM_WSOCK].cmd,API_CMD_LEN);
	QUEUE_IP_BYTES(buff,SPI_PAYLOAD_POS + API_CMD_LEN + WSOCK_SIP_POS,ServerIP);
	QUEUE_PORT_BYTES(buff,SPI_PAYLOAD_POS + API_CMD_LEN + WSOCK_SPO_POS,ServerPort);
	buff[SPI_PAYLOAD_POS + API_CMD_LEN + WSOCK_SKN_POS] = socketNumber;
	buff[SPI_PAYLOAD_POS + API_CMD_LEN + WSOCK_TIP_POS] = (uint8_t)Socket_flag;
	buff[SPI_PAYLOAD_POS + API_CMD_LEN + WSOCK_CON_POS] = conf;

	return Api_callback_Array[ENUM_WSOCK].cmd_len + API_CMD_LEN;
}


/**
  * @brief  Make the WTXCM data buffer
  * @param  buff: pointer to TX buffer
  * @retval buffer_size
  */
uint16_t Gen_WTXCM( uint8_t* buff, uint8_t socketNumber, uint16_t datasize, uint8_t* data){

	if (!WSOCK_SKN_ASSERT(socketNumber)){
		/* socketNumber command wrong */
		SPI_EXE_ERROR();
	}
	if (!WTXCM_DTS_ASSERT(datasize)){
		/* conf command wrong */
		SPI_EXE_ERROR();
	}
	SET_TX_SIZE(buff,(Api_callback_Array[ENUM_WTXCM].cmd_len + API_CMD_LEN + datasize));

	memcpy(&buff[SPI_PAYLOAD_POS],Api_callback_Array[ENUM_WTXCM].cmd,API_CMD_LEN);
	buff[SPI_PAYLOAD_POS + API_CMD_LEN + WTXCM_SKN_POS] = socketNumber;
	QUEUE_DATA_SIZE(buff,SPI_PAYLOAD_POS + API_CMD_LEN + WTXCM_DTS_POS,datasize);

	/* Mem copy - mem 2 mem */
	SPI_HS_M2M((uint8_t *)&data[0], (uint8_t *)&buff[SPI_PAYLOAD_POS + API_CMD_LEN + WTXCM_DAT_POS], datasize);

	return Api_callback_Array[ENUM_WTXCM].cmd_len + API_CMD_LEN + datasize;
}

/**
  * @brief  Return the pointer to WTXCM data
  * @param  buff: pointer to TX buffer
  * @retval buffer_size
  */
uint16_t Gen_WTXCM_Pointer( uint8_t* buff, uint8_t socketNumber, uint16_t datasize, uint8_t** data){

	if (!WSOCK_SKN_ASSERT(socketNumber)){
		/* socketNumber command wrong */
		SPI_EXE_ERROR();
	}
	if (!WTXCM_DTS_ASSERT(datasize)){
		/* conf command wrong */
		SPI_EXE_ERROR();
	}
	SET_TX_SIZE(buff,(Api_callback_Array[ENUM_WTXCM].cmd_len + API_CMD_LEN + datasize));

	memcpy(&buff[SPI_PAYLOAD_POS],Api_callback_Array[ENUM_WTXCM].cmd,API_CMD_LEN);
	buff[SPI_PAYLOAD_POS + API_CMD_LEN + WTXCM_SKN_POS] = socketNumber;
	QUEUE_DATA_SIZE(buff,SPI_PAYLOAD_POS + API_CMD_LEN + WTXCM_DTS_POS,datasize);

	/* pointer to packet payload */
	*data = (buff + SPI_PAYLOAD_POS + API_CMD_LEN + WTXCM_DAT_POS);

	return Api_callback_Array[ENUM_WTXCM].cmd_len + API_CMD_LEN + datasize;
}

/**
  * @brief  Make the WTXDT data buffer
  * @param  buff: pointer to TX buffer
  * @retval buffer_size
  */
uint16_t Gen_WTXDT( uint8_t* buff, uint8_t* data, uint16_t datasize){

	if (!WTXCM_DTS_ASSERT(datasize)){
		/* conf command wrong */
		SPI_EXE_ERROR();
	}

	memcpy(buff,Api_callback_Array[ENUM_WTXDT].cmd,API_CMD_LEN);
	memcpy(&buff[API_CMD_LEN],data,datasize);

	return datasize + API_CMD_LEN;
}



/**
  * @brief  DataReadyCallback, must be used in GPIO Interrupt callback
  * @param  None
  * @retval None
  */
void DataReadyPINClbk( void ){
	rx_dataready = TRUE;
}

/**
  * @brief  Data ready read
  * @param  None
  * @retval None
  */
uint8_t Is_DataReady( void ){
	return rx_dataready | IS_SLAVE_DATA_READY();
}




/**
  * @brief   Convert the character string in "ip" into an unsigned integer.
  * '0.0.0.0' is not a valid IP address, so this uses the value 0 to indicate an invalid IP address.
  * @param  IP string
  * @retval None
  */
uint32_t ip_to_int (const char * ip)
{
    /* The return value. */
    unsigned v = 0;
    /* The count of the number of bytes processed. */
    int i;
    /* A pointer to the next digit to process. */
    const char * start;

    start = ip;
    for (i = 0; i < 4; i++) {
        /* The digit being processed. */
        char c;
        /* The value of this byte. */
        int n = 0;
        while (1) {
            c = * start;
            start++;
            if (c >= '0' && c <= '9') {
                n *= 10;
                n += c - '0';
            }
            /* We insist on stopping at "." if we are still parsing
               the first, second, or third numbers. If we have reached
               the end of the numbers, we will allow any character. */
            else if ((i < 3 && c == '.') || i == 3) {
                break;
            }
            else {
                return FALSE;
            }
        }
        if (n >= 256) {
            return FALSE;
        }
        v *= 256;
        v += n;
    }
    return v;
}

// ---------------------------------------------------



// RV -------------------------------------------------------------------


static pi_task_t cb_gpio;






void MX_GPIO_Init( void ){

	int32_t errors = 0;

	struct pi_gpio_conf gpio_conf;
	
	pi_gpio_conf_init(&gpio_conf);
    	pi_open_from_conf(&gpio, &gpio_conf);
    	errors = pi_gpio_open(&gpio);
    	if (errors)
    	{
        	printf("Error opening GPIO %d\n", errors);
        	pmsis_exit(errors);
    	}
    

	//GPIO 19 exposed on PAD 33
	pi_gpio_flags_e cfg_flags = PI_GPIO_INPUT;
	pi_gpio_pin_configure(&gpio, SPI_BUSY_PI, cfg_flags);
	
	
	//GPIO 5 exposed on PAD 17
    	pi_gpio_notif_e irq_type = PI_GPIO_NOTIF_RISE;
    	cfg_flags = PI_GPIO_INPUT;

    	pi_task_callback(&cb_gpio, HAL_GPIO_EXTI_Callback, 0);
    	
    	/* Configure gpio input. */
    	pi_gpio_pin_configure(&gpio, SPI2_DATAREADY_PI, cfg_flags);
   	pi_gpio_pin_task_add(&gpio, SPI2_DATAREADY_PI, &cb_gpio, irq_type);
	pi_gpio_pin_notif_configure(&gpio, SPI2_DATAREADY_PI, irq_type);

    
	/* WIFI PWR */
	// GPIO initialization
	cfg_flags = PI_GPIO_OUTPUT;
	pi_gpio_pin_configure(&gpio, SPI2_PWR_PI, cfg_flags);
	pi_gpio_pin_write(&gpio, SPI2_PWR_PI, 0);
  

	/* WIFI NRST */
	cfg_flags = PI_GPIO_OUTPUT;
	pi_gpio_pin_configure(&gpio, SPI2_RST_PI, cfg_flags);
	pi_gpio_pin_write(&gpio, SPI2_RST_PI, 0);

	printf("Configuration of GPIO for WiFi done!\n\r" );

}



void MX_SPI_Init(void){
	/* First configure the SPI device */
	struct pi_spi_conf spi_conf={0};
	
	/* Get default configuration */
	pi_spi_conf_init(&spi_conf);
	
	/* Set baudrate. Can actually be lower than
   	that depending on the best divider found */
	spi_conf.max_baudrate = SPI_BAUDRATE;
	/* SPI interface identifier as the Pulp chip can have
   	several interfaces */
	spi_conf.itf = 1;
	// Chip select
	spi_conf.cs = 0;
	spi_conf.wordsize = PI_SPI_WORDSIZE_8 ;

	/* Then open the device */
	pi_open_from_conf(&spim, &spi_conf);
	if (pi_spi_open(&spim))
	      {printf("SPI open failed"); pmsis_exit(-1);}
	      
	   

	
	printf("Configuration of SPI done!!!\n\r" );
	printf("SPI at %d bps \n\r" , SPI_BAUDRATE);
}


/**
 * @brief  Start wait timer for WiFi answer
 *
 * @param timeout: number of ms
 * @retval None
 */
void Start_Timer(uint32_t timeout){
	wait_timer = TIME_ELAPSED_MS() + timeout;
}

/**
 * @brief  Check wait timer for WiFi answer
 *
 * @param None
 * @retval TRUE if elapsed
 */
uint8_t Is_Timer_Elapsed (void){
	if(wait_timer < TIME_ELAPSED_MS()){
		return TRUE;
	}else{
		return FALSE;
	}
}

/**
 * @brief  EXTI line detection callback.
 * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(void *arg)
{ 

	DataReadyPINClbk();

}




/**
 * @brief  Memcpy optimized for int32
 * @param  str1: Destination pointer
 * @param  str2: Source pointer
 * @param  n:    Number of integer (int32) to be copied
 * @retval None
 */
void memcpy_int(uint32_t *str1, const uint32_t *str2, size_t n)
{
	while(n--){
		*str1++ = *str2++;
	}
}


/**
 * @brief  This function is executed in case of error occurrence.
 * @param  file: The file name as string.
 * @param  line: The line in file as a number.
 * @retval None
 */
void _Error_Handler(char *file, int line)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while(1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}



//---------------------------------------------------


