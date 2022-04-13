/* MultiRadio_library.h
 *  Multiradio support - Roberto Vezzani - using new spi libraries
 *  Tested wifi parts online
 *  Multiradio callbacks - OK
 *  basic MACRO redefined with inline functions
 */
 
 //#include "MultiRadio_main.h"
 #include "MultiRadio_defines.h"

#ifndef MULTIRADIO_LIBRARY_RV_H_
#define MULTIRADIO_LIBRARY_RV_H_


#ifndef TRUE
#define TRUE						(1)
#endif
#ifndef FALSE
#define FALSE						(0)
#endif

#ifndef __weak
	#define __weak   __attribute__((weak))
#endif /* __weak */
#ifndef __packed
	#define __packed __attribute__((__packed__))
#endif /* __packed */
#ifndef UNUSED
	#define UNUSED(x) ((void)(x))
#endif



#ifndef DEBUGRV
    #define printd(fmt, ...)  do { } while(0)
#else
    #define printd(fmt, ...) printf(fmt, __VA_ARGS__)
#endif



#define MAKE_IPV4_ADDR(p,pos)		MAKE_IPV4_ADDRESS(p[pos], p[pos+1], p[pos+2], p[pos+3])
#define MAKE_IPV4_PORT(p,pos)		(((uint16_t)p[pos] << 8) | ((uint16_t)p[pos+1]))


/******************************************************
 *               Socket Definitions
 ******************************************************/
#define MAX_NUM_SOCKET_SUPPORTED		(10)

/******************************************************
 *               Packet Definitions
 ******************************************************/
/* in bytes */
#define PACKET_MAX_SIZE_WIFIAPP			(1470)
#define API_CMD_LEN						(5)

/* api costructors */
#define CONCAT(a,b)							a ## b
#define API_FUNCTION(cmd)  					CONCAT(clbk_,cmd)
#define API_NAME(cmd)						#cmd
#define EXPAND_AS_API_INIT_STR(cmd,len)		{API_FUNCTION(cmd),API_NAME(cmd),len},
#define EXPAND_AS_API_INIT_FUN(cmd,len)		void API_FUNCTION(cmd)  (uint8_t * buff);
#define EXPAND_AS_API_WEAK_FUN(cmd,len)		__weak void API_FUNCTION(cmd)  (uint8_t * buff) {UNUSED(buff);};
#define EXPAND_AS_API_ENUM(cmd,len)			CONCAT(ENUM_,cmd) ,
#define API_ARRAY_SIZE(n)					(n+1)


#define TIME_ELAPSED_MS()   ((uint32_t)(pi_time_get_us () / 1000))
#define DELAY_MS(ms)        pi_time_wait_us (ms * 1000)

typedef struct {
	void (*Api_Callback)(uint8_t * buff);
	char cmd[API_CMD_LEN+1];
	uint16_t cmd_len;
}Api_callback_t;


/* INSERT HERE -----------------------------------------------------------------*/
/* insert here the command identifier and the expected data length */
#define CMD_TABLE(ENTRY)			\
		ENTRY( WPWRC , 1)					\
		ENTRY( WSSID , 97)					\
		ENTRY( WSSIU , 0)					\
		ENTRY( WSOCK , 9)					\
		ENTRY( WTXCM , 5)					\
		ENTRY( WRXCM , 1)					\
		ENTRY( WTXDT , 0)					\
		ENTRY( ACKOK , 0)					\
		ENTRY( ACKNO , 0)					\
		ENTRY( WNTUP , 0)					\
		ENTRY( WNTDW , 0)					\
		ENTRY( WTXOK , 0)					\
		ENTRY( WTXNO , 0)					\
		ENTRY( BPWRC , 1)					\
		ENTRY( BTXCM , 1)					\
		ENTRY( BRXCM , 1)					\
		ENTRY( BNTUP , 0)					\
		ENTRY( BNTDW , 0) 					\
		ENTRY( LPWRC , 1)					\
		ENTRY( LNKEY , 32)					\
		ENTRY( LTXCM , 2)					\
		ENTRY( LNTUP , 0)					\
		ENTRY( LNTDW , 0)					\


/******************************************************
 *               API Packet Pointers
 ******************************************************/


/* WPWRC */
#define WSSID_PWR_POS					(0)

#define WPWRC_PWR_ASSERT(x)				(x < 5)

typedef enum {
	APPWIFI_LOWPOWERMODE_RESET = 0,
	APPWIFI_LOWPOWERMODE_LPCO,
	APPWIFI_LOWPOWERMODE_LPOS,
	APPWIFI_LOWPOWERMODE_HTCO,
	APPWIFI_LOWPOWERMODE_HTOS
}AppWiFi_LowPowerMode_e;

/* WSSID */
/*
AP WiFi Name (String)					[5 - 34]
AP WiFi Password (String)				[35 - 69]
Device Name (String)					[70 - 79]
DHCP ENABLED [TRUE,FALSE]					80
IP - MSB first							[81 - 84]
Subnet Mask - MSB first					[85 - 88]
Default Gateway - MSB first				[89-92]
DNS1 - MSB first						[93-96]
DNS2 - MSB first						[97-100]
*/
#define WSSID_APN_POS					(0)
#define WSSID_APP_POS					(30)
#define WSSID_DNA_POS					(65)
#define WSSID_DHC_POS					(75)
#define WSSID_IPM_POS					(76)
#define WSSID_SBM_POS					(80)
#define WSSID_GAT_POS					(84)
#define WSSID_DN1_POS					(88)
#define WSSID_DN2_POS					(92)
#define WSSID_SEC_POS					(96)

#define WSSID_APN_LEN					(29)
#define WSSID_APP_LEN					(29)
#define WSSID_DNA_LEN					(9)

#define WSSID_DHCP_ASSERT(x)			(x < 2)
#define WSSID_SECURITY_ASSERT(x)		(x < 2)
#define WSSID_APNLEN_ASSERT(x)			(x < WSSID_APN_LEN)
#define WSSID_APPLEN_ASSERT(x)			(x < WSSID_APP_LEN)

typedef enum {
	SSID_SECURITY_OPEN = 0,
	SSID_SECURITY_WPA2
}AppWiFi_SSID_security_e;

/* WSOCK */
#define WSOCK_SIP_POS					(0)
#define WSOCK_SPO_POS					(4)
#define WSOCK_SKN_POS					(6)
#define WSOCK_TIP_POS					(7)
#define WSOCK_CON_POS					(8)

#define WSOCK_SKN_ASSERT(x)				(x < MAX_NUM_SOCKET_SUPPORTED)
#define WSOCK_TIP_ASSERT(x)				(x < 2)
#define WSOCK_CON_ASSERT(x)				(x < 2)

typedef enum {
	SCK_UDP = 0,
	SCK_TCP
}Socket_flag_e;

/* WTXCM */
#define WTXCM_SKN_POS					(0)
#define WTXCM_DTS_POS					(1)
#define WTXCM_DAT_POS					(5)

#define WTXCM_SKN_ASSERT(x)				(x < MAX_NUM_SOCKET_SUPPORTED)
#define WTXCM_DTS_ASSERT(x)				(x <= PACKET_MAX_SIZE_WIFIAPP)

/* WRXCM */
#define WRXCM_SKN_POS		     		(0)
#define WRXCM_DTS_POS		     		(1)
#define WRXCM_DAT_POS		     		(5)

#define WTXCM_DTS_ASSERT(x)				(x <= PACKET_MAX_SIZE_WIFIAPP)

/* BPWRC */
#define BPWRC_PWR_POS					(0)

#define BPWRC_PWR_ASSERT(x)				(x < 2)

/* BTXCM */
#define BTXCM_DTS_POS					(0)
#define BTXCM_PAY_POS					(1)

#define BTXCM_DTS_ASSERT(x)				(x <= MAX_SIZE_BLEAPP)

/* BRXCM */
#define BRXCM_DTS_POS					(0)
#define BRXCM_PAY_POS					(1)

#define BRXCM_DTS_ASSERT(x)				(x <= MAX_SIZE_BLEAPP)

/* LPWRC */
#define LPWRC_PWR_POS					(0)

#define LPWRC_PWR_ASSERT(x)				(x < 2)

/* LNKEY */
#define LNKEY_DEV_POS                   (0)
#define LNKEY_APPID_POS                 (8)
#define LNKEY_APPKEY_POS                (16)

#define LNKEY_DEV_LEN                   (8)
#define LNKEY_APPID_LEN                 (8)
#define LNKEY_APPKEY_LEN                (16)

#define LNKEY_DTS_ASSERT(x)             (x < LNKEY_DEV_LEN+LNKEY_APPID_LEN+LNKEY_APPKEY_LEN)

/* LTXCM */
#define LTXCM_POR_POS					(0)
#define LTXCM_DTS_POS					(1)
#define LTXCM_PAY_POS					(2)

#define LTXCM_DTS_ASSERT(x)				(x <= MAX_SIZE_LORAPP)

/* PRIVATE FUNCTIONS -----------------------------------------------------------------*/
/* PRIVATE FUNCTIONS -----------------------------------------------------------------*/
/* PRIVATE FUNCTIONS -----------------------------------------------------------------*/
/* PRIVATE FUNCTIONS -----------------------------------------------------------------*/
/* PRIVATE FUNCTIONS -----------------------------------------------------------------*/
/* PRIVATE FUNCTIONS -----------------------------------------------------------------*/

/* SPI transfer buffer size */
#define SPI_LEN_POS					(0)
#define SPI_PAYLOAD_POS				(2)
#define SPI_BUFF_DIM				(PACKET_MAX_SIZE_WIFIAPP + API_CMD_LEN + SPI_PAYLOAD_POS + 10)

enum {
	ENUM_API_START = -1 ,
	CMD_TABLE(EXPAND_AS_API_ENUM)
	ENUM_API_SIZE
};


CMD_TABLE(EXPAND_AS_API_INIT_FUN)


extern const Api_callback_t Api_callback_Array[];
extern const uint8_t API_CMD_STR_SIZE;

void MultiRadio_Init ( void );

uint16_t SPI_ExtInterface ( uint8_t* tx , uint8_t* rx, uint16_t tx_size);
void ExecuteCommandClbk(uint8_t* rx, int16_t rx_data_size);

uint16_t Gen_WPWRC(uint8_t* buff, AppWiFi_LowPowerMode_e pwrmode);
uint16_t Gen_WSSID(uint8_t* buff, char* AppWiFiName, char* AppWiFiPassword, char* DeviceName, uint8_t DHCP,
		uint32_t IP, uint32_t subnetmask, uint32_t DefaultGateway, uint32_t DNS1, uint32_t DNS2,
		AppWiFi_SSID_security_e security);
uint16_t Gen_WSOCK( uint8_t* buff, uint32_t ServerIP, uint16_t ServerPort,
		uint8_t socketNumber, Socket_flag_e Socket_flag, uint8_t conf);
uint16_t Gen_WTXCM( uint8_t* buff, uint8_t socketNumber, uint16_t datasize, uint8_t* data);
uint16_t Gen_WTXCM_Pointer( uint8_t* buff, uint8_t socketNumber, uint16_t datasize, uint8_t** data);
uint16_t Gen_WTXDT( uint8_t* buff, uint8_t* data, uint16_t datasize);
uint16_t Gen_BPWRC( uint8_t* buff, uint8_t pwrmode);
uint16_t Gen_BTXCM( uint8_t* buff, uint8_t datasize, uint8_t* data);
uint16_t Gen_LPWRC( uint8_t* buff, uint8_t pwrmode);
uint16_t Gen_LNKEY( uint8_t* buff, uint8_t* devEUI, uint8_t* appID, uint8_t* appKEY );
uint16_t Gen_LTXCM( uint8_t* buff, uint8_t port, uint8_t datasize, uint8_t* data, uint8_t headersize, uint8_t* header);
void DataReadyPINClbk( void );
uint8_t Is_DataReady( void );
uint32_t ip_to_int (const char * ip);



void MX_GPIO_Init( void );
void MX_SPI_Init(void);

void Start_Timer(uint32_t timeout);
uint8_t Is_Timer_Elapsed (void);
void HAL_GPIO_EXTI_Callback(void *arg);

void _Error_Handler(char *file, int line);



void InitBuffers();






uint8_t max_value_byte(uint8_t *buf, int len);
uint8_t bit5_value_byte(uint8_t *buf, int len);
void Send_Img_Raw(uint8_t *img, uint32_t size);
void memcpyDMA(uint32_t *str1, const uint32_t *str2, size_t n);








#endif /* MULTIRADIO_LIBRARY_H_ */
