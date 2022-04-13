/*
 * Copyright 2019 GreenWaves Technologies, SAS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/* PMSIS includes. */
#include "pmsis.h"

/* PMSIS BSP includes. */
#include "bsp/bsp.h"
#include "bsp/camera.h"

/* Demo includes. */
#include "gaplib/ImgIO.h"


#ifdef USE_QSPI
# include "bsp/flash/spiflash.h"
# include "bsp/ram/spiram.h"
#else
# include "bsp/flash/hyperflash.h"
# include "bsp/ram/hyperram.h"
#endif


#include "MultiRadio_AHT.h"

#include "RVhelpers.h"

#include "main.h"
#include "modelInfo.h"
#include "aht_defines.h"


# 
/* Defines */
#define HM 80*64*2
#define SZ 80*64
#define AT_INPUT_SIZE 	(AT_INPUT_WIDTH*AT_INPUT_HEIGHT*AT_INPUT_COLORS)

#define myFIX2FP(Val, Precision)    ((float) (Val) / (float) (1<<(Precision)))

#define __XSTR(__s) __STR(__s)
#define __STR(__s) #__s


#ifdef HAVE_LCD
	#define LCD_WIDTH    AT_INPUT_WIDTH
	#define LCD_HEIGHT   AT_INPUT_HEIGHT
#endif

#if PERF == 0
#undef PERF
#endif


//typedef signed short int NETWORK_OUT_TYPE;
typedef signed char NETWORK_OUT_TYPE;

// Global Variables
static struct pi_device camera;

//static pi_buffer_t buffer;

#ifdef USE_QSPI
struct pi_device QspiRam;
#define EXTERNAL_RAM QspiRam
#else
struct pi_device HyperRam;
#define EXTERNAL_RAM HyperRam
#endif


L2_MEM NETWORK_OUT_TYPE *hm_ResOut=0;
L2_MEM NETWORK_OUT_TYPE *sz_ResOut=0;

	
uint8_t  *pimgBufferCamera;   // image from the camera  (to send using the net)

static uint32_t pimgPadded; // padded image to provide as input to the network

AT_HYPERFLASH_FS_EXT_ADDR_TYPE AT_L3_ADDR = 0;


struct pi_device cluster_dev;
// ------------------------------------

static void printMem(){
  uint8_t * tmp=0;
  uint32_t sizeToAllocate=10*WcamCrop*HcamCrop*sizeof(char);
  printf("start from size %d\n",sizeToAllocate);
  while (!tmp)
  	{
  	tmp = (uint8_t*) pmsis_l2_malloc(sizeToAllocate);
  	if (!tmp) sizeToAllocate -= 100;
  	}
  printf("allocated size %d\n",sizeToAllocate);

  pmsis_l2_malloc_free(tmp,sizeToAllocate);
  }
  
  
  

// RV  Input camera -------------------------------------------
static int32_t InitAndOpenCamera(struct pi_device *deviceCam)
{
  	 
  
  // Allocate L2 buffers to store temp input data from camera
  pimgBufferCamera = (uint8_t*) pmsis_l2_malloc(WcamCrop*HcamCrop*sizeof(char));
  
  //Allocate L3 buffer for network input
  if (pi_ram_alloc(&EXTERNAL_RAM, &pimgPadded, (uint32_t) (3*WcamPadded*HcamPadded*sizeof(char))))
	{
		printf("Ram malloc failed !\n");
		pmsis_exit(-4);
	}

  
      if (pimgBufferCamera == NULL) 
    {
        printf("Failed to allocate Memory for Image 1\n");
        pmsis_exit(-1);
    }
    
     if (pimgPadded == NULL) 
    {
        printf("Failed to allocate Memory for Image 2\n");
        pmsis_exit(-1);
    }
    
    	
	//init padded image
	//memset(pimgPadded,0,WcamPadded*HcamPadded*sizeof(char));
	
	uint8_t* tmp = (uint8_t*) pmsis_l2_malloc(WcamPadded);
	memset(tmp,0,WcamPadded *sizeof(char));
	for(int i=0; i<3*HcamPadded; i++)
  		pi_ram_write	(&EXTERNAL_RAM, pimgPadded+i*WcamPadded,tmp ,WcamPadded);
  	pmsis_l2_malloc_free(tmp,WcamPadded);
	printf("Init padded image finished\n");
        

	struct pi_himax_conf cam_conf;
	pi_himax_conf_init(&cam_conf);
	

	cam_conf.roi.slice_en = 1;
	

	// TODO: PARAMETRI
	cam_conf.roi.x = (int)((Wcam-WcamCrop)/2);
	cam_conf.roi.y = (int)((Hcam-HcamCrop)/2);
	cam_conf.roi.w = WcamCrop;
	cam_conf.roi.h = HcamCrop;

	pi_open_from_conf(deviceCam, &cam_conf);
	if (pi_camera_open(deviceCam))
	{
	return -1;
	}

	/* Let the camera AEG work for 100ms */
	// TODO?
	pi_camera_control(deviceCam, PI_CAMERA_CMD_AEG_INIT, 0);
	pi_camera_control(deviceCam, PI_CAMERA_CMD_TRIGGER_MODE, 0);

    
    return 0;
}


int CaptureImage(){	


	printf("Reading frame \r\n");


  
    

	pi_camera_control(&camera, PI_CAMERA_CMD_START, 0);
	pi_camera_capture(&camera, pimgBufferCamera, WcamCrop*HcamCrop);
	pi_camera_control(&camera, PI_CAMERA_CMD_STOP, 0);

	printf("Frame ok \r\n");

	uint32_t x,y, ch;
	uint32_t xPad;
    	uint32_t yPad;
    	
    	if (ROTATEDCAMERA)
    	{
    		xPad=(uint32_t)((WcamPadded-HcamCrop)/2);
    		yPad= (uint32_t)((HcamPadded-WcamCrop)/2);
    		
    		for  (ch=0; ch<3; ch++)
    		{	uint32_t offset= ch*WcamPadded*HcamPadded;
	    		for (y=0;y<HcamCrop;y++)
				for (x=0;x<WcamCrop;x++)
					pi_ram_write (&EXTERNAL_RAM, pimgPadded+offset + y +xPad+ (HcamPadded-x-yPad-1)*WcamPadded,pimgBufferCamera+y*WcamCrop+x ,1);
					//pi_ram_write	(&EXTERNAL_RAM, pimgPadded+offset + y +xPad+ (x+yPad)*WcamPadded,pimgBufferCamera+y*WcamCrop+x ,1);
					//*((uint8_t*)(pimgPadded+offset + y + x*WcamPadded))= pimgBufferCamera[y*WcamCrop+x];
		}
		printf("Frame rotated ok \r\n");	
    	}
    	else
    	{
    		xPad=(uint32_t)((WcamPadded-WcamCrop)/2);
    		yPad= (uint32_t)((HcamPadded-HcamCrop)/2);
    		
    		for  (ch=0; ch<3; ch++)
    		{	uint32_t offset= ch*WcamPadded*HcamPadded;
	    		for (y=0;y<HcamCrop;y++)
				for (x=0;x<WcamCrop;x++)
					pi_ram_write (&EXTERNAL_RAM, pimgPadded+offset + x +xPad+ (y+yPad)*WcamPadded,pimgBufferCamera+y*WcamCrop+x ,1);
					//pi_ram_write	(&EXTERNAL_RAM, pimgPadded+offset + y +xPad+ (x+yPad)*WcamPadded,pimgBufferCamera+y*WcamCrop+x ,1);
					//*((uint8_t*)(pimgPadded+offset + y + x*WcamPadded))= pimgBufferCamera[y*WcamCrop+x];
		}
			
    	}
	/*RV TODO: Optimize 
	for (y=0;y<HcamCrop;y++)
		for (x=0;x<WcamCrop;x++)
			pimgPadded[(y+yPad)*WcamCrop+(x+xPad)] = pimgBufferCamera[y*WcamCrop+x];
			
	*/		
		
		
	// Copy Single Channel Greyscale to 3 channel RGB: CH0-CH1-CH2.
	/*
	
	pi_ram_write(&EXTERNAL_RAM, (pimgPadded), pimgBufferCamera, (uint32_t) AT_INPUT_WIDTH*AT_INPUT_HEIGHT);
	pi_ram_write(&EXTERNAL_RAM, (pimgPadded+WcamPadded*HcamPadded), pimgBufferCamera, (uint32_t) AT_INPUT_WIDTH*AT_INPUT_HEIGHT);
	pi_ram_write(&EXTERNAL_RAM, (pimgPadded+2*WcamPadded*HcamPadded), pimgBufferCamera, (uint32_t) AT_INPUT_WIDTH*AT_INPUT_HEIGHT);
	*/


	return 0;
}



static void RunNetwork()
{
    printf("Running on cluster\n");
	#ifdef PERF
	    printf("Start timer\n");
	    gap_cl_starttimer();
	    gap_cl_resethwtimer();
	#endif
    AT_CNN((unsigned char *) pimgPadded, hm_ResOut, sz_ResOut);
    printf("Runner completed\n");
}



//allocate and deallocate output vectors
int AllocOutput(){

	// Allocate the output tensor, only the first time
	
	if (hm_ResOut==0)
		hm_ResOut = (NETWORK_OUT_TYPE *) AT_L2_ALLOC(0, HM*sizeof(NETWORK_OUT_TYPE));
	if (hm_ResOut==0) {
		printf("Failed to allocate Memory for Result 0 (%ld bytes)\n", 2*sizeof(char));
		return 1;
	}

	if (sz_ResOut==0)
		sz_ResOut = (NETWORK_OUT_TYPE *) AT_L2_ALLOC(0, SZ*sizeof(NETWORK_OUT_TYPE));
        if (sz_ResOut==0) {
                printf("Failed to allocate Memory for Result 1 (%ld bytes)\n", 2*sizeof(char));
                return 1;
        }
        return 0;
 }
 
int DeallocOutput(){
	pmsis_l2_malloc_free(hm_ResOut, HM*sizeof(NETWORK_OUT_TYPE));
	hm_ResOut=0;
	pmsis_l2_malloc_free(sz_ResOut, SZ*sizeof(NETWORK_OUT_TYPE));
	sz_ResOut=0;
	return 0;
}


 int SendStatistics(uint16_t counterMASK,uint16_t counterNoMASK){
		uint8_t buffer[100];
		sprintf(buffer,"%d, %d \n", counterMASK,counterNoMASK);
		WIFI_sendData(buffer, 100, 1,0);
		return 0;
	}
	
	
  int SendHeatMaps(uint8_t * hm_ResOut, uint8_t* sz_ResOut){
	// create buffer 
	uint8_t *buffer;	
	buffer= pmsis_l2_malloc(HM+SZ);
	
	memcpy(buffer, hm_ResOut, HM);
	memcpy(buffer+HM, sz_ResOut, SZ);
	
	
	/*
	int i;
	for (i=0; i<HM; i++)
	{buffer[i] =(uint8_t) (255.0* ((((float)hm_ResOut[i]) - 14)*0.00486328)) ;    // TODO: quant
	} 
	for (i=HM; i<HM+SZ; i++)
	{buffer[i] = (uint8_t) (255.0 * ((((float)sz_ResOut[i]) - 4)*0.00421687) );  // TODO: quant
	}*/
	
	
	WIFI_sendData(buffer, HM+SZ, 2,0);
	pmsis_l2_malloc_free(buffer, HM+SZ);
  }




 int ElabImage()
 
 {
	   
	// Task setup
	struct pi_cluster_task *task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
	if(task==NULL) {
	  printf("pi_cluster_task alloc Error!\n");
	  pmsis_exit(-1);
	}
	printf("Stack size is %d and %d\n",STACK_SIZE,SLAVE_STACK_SIZE );
	memset(task, 0, sizeof(struct pi_cluster_task));
	task->entry = &RunNetwork;
	task->stack_size = STACK_SIZE;
	task->slave_stack_size = SLAVE_STACK_SIZE;
	task->arg = NULL;
	
   


	printf("running network...\n");

	// Dispatch task on the cluster

	pi_cluster_send_task_to_cl(&cluster_dev, task);


	printf("analysis of the results\n");

	//Check Results
	#define IDMASK (0)
	#define IDNOMASK (1)
	
	// invece di fare maxpool, vado direttamente a leggere tutta la matrice
	uint16_t counterMASK=0;
	uint16_t counterNoMASK=0;
	
	float tmpval;
	float minvalM, maxvalM;
	minvalM=999;
	maxvalM=0;
	float minvalNM, maxvalNM;
	minvalNM=999;
	maxvalNM=0;
	
	
	for(int i=0; i<SZ; i++){
		tmpval=myFIX2FP(hm_ResOut[i+IDMASK*SZ] * model_Output_1_OUT_QSCALE, model_Output_1_OUT_QNORM);
		if (minvalM>tmpval) minvalM=tmpval;
		if (maxvalM<tmpval) maxvalM=tmpval;
		
		if (tmpval > 0.1){
			counterMASK = counterMASK + 1;
			//printf("M pos:%d, val:%f\n",i+IDMASK*SZ, tmpval);  		
		}
		tmpval=myFIX2FP(hm_ResOut[i+IDNOMASK*SZ] * model_Output_1_OUT_QSCALE, model_Output_1_OUT_QNORM);
		if (minvalNM>tmpval) minvalNM=tmpval;
		if (maxvalNM<tmpval) maxvalNM=tmpval;
		
		if (tmpval> 0.1){
			counterNoMASK = counterNoMASK + 1;		
			//printf("NM pos:%d, val:%f\n",i+IDNOMASK*SZ, tmpval);
		}
	}
	
		
	printf("Model:\t%s\n\n", __XSTR(AT_MODEL_PREFIX));
	printf("counterMASK:\t%d\n", counterMASK);
	printf("counterNoMASK:\t%d\n", counterNoMASK);
	printf("min M:%f, maxM: %f\n",minvalM,maxvalM);
	printf("min NM:%f, maxNM: %f\n\n",minvalNM,maxvalNM);
	
	SendStatistics(counterMASK,counterNoMASK);
	SendHeatMaps(hm_ResOut,sz_ResOut);
	
	

	// Performance counters
#ifdef PERF
	{
		unsigned int TotalCycles = 0, TotalOper = 0;
		printf("\n");
		for (int i=0; i<(sizeof(AT_GraphPerf)/sizeof(unsigned int)); i++) {
			printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\n", AT_GraphNodeNames[i], AT_GraphPerf[i], AT_GraphOperInfosNames[i], ((float) AT_GraphOperInfosNames[i])/ AT_GraphPerf[i]);
			TotalCycles += AT_GraphPerf[i]; TotalOper += AT_GraphOperInfosNames[i];
		}
		printf("\n");
		printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\n", "Total", TotalCycles, TotalOper, ((float) TotalOper)/ TotalCycles);
		printf("\n");
	}
#endif

	pmsis_l2_malloc_free(task, sizeof(struct pi_cluster_task));
	
	if (counterMASK+counterNoMASK>100)
		return 1;
	
	
	return 0;
}



 


int body(void)
{	int ris;
	
	// RV -  INIT COMMON PART
	
	
	// Voltage-Frequency settings
	uint32_t voltage =1200;
	pi_freq_set(PI_FREQ_DOMAIN_FC, FREQ_FC*1000*1000);
	pi_freq_set(PI_FREQ_DOMAIN_CL, FREQ_CL*1000*1000);
	
	DELAY_MS(10);
	
	//PMU_set_voltage(voltage, 0);
	printf("Set VDD voltage as %.2f, FC Frequency as %d MHz, CL Frequency = %d MHz\n", (float)voltage/1000, FREQ_FC, FREQ_CL);
	printMem();
	

	
	// Initialize the hyperram
	struct pi_hyperram_conf hyper_conf;
	pi_hyperram_conf_init(&hyper_conf);
	pi_open_from_conf(&EXTERNAL_RAM, &hyper_conf);
	if (pi_ram_open(&EXTERNAL_RAM))
	{
		printf("Error ram open !\n");
		pmsis_exit(-3);
	}
	printf("ram opened !\n");
	
	
	
	// Open the cluster
	struct pi_cluster_conf conf;
	pi_cluster_conf_init(&conf);
	pi_open_from_conf(&cluster_dev, (void *)&conf);
	if (pi_cluster_open(&cluster_dev))
        {
            printf("Cluster open failed !\n");
            pmsis_exit(-4);
        }
	printf("Cluster opened!\n");
	
	
	if (InitAndOpenCamera(&camera))
	{
		printf("Error open camera!\n");
		pmsis_exit(-7);
	}
	printf("camera opened!\n");

	
	

	DELAY_MS(10);
	if (AT_CONSTRUCT())
	{
	  printf("Graph constructor exited with error: %d\n", ris);
	  pmsis_exit(-5);
	}
	printf("Network Constructor was OK!\n");
	
	if (AllocOutput()){
		printf("Error with output allocation\n");
		pmsis_exit(-7);
	}
	
	
  	DELAY_MS(10); 
   	// Init multiradio and wifi
	initMultiradio();
	initWiFi();
	
 	DELAY_MS(10);	
 	
 	printMem(); 

	
	while(1){

		if (CaptureImage()){
			printf("Error capturing the image\n\r" );
			DELAY_MS(500); 
			}
		else
		{	
			printf("Image processing\n\r" );
			
			// face detection 
			int risNBB = ElabImage();
			printf("found %d faces \n",risNBB);
			if (risNBB>0){
				// pimgBufferCamera, WcamCrop*HcamCrop);
				//WIFI_sendImage(pimgBufferCamera, WcamCrop*HcamCrop, 0 );
				WIFI_sendData(pimgBufferCamera, WcamCrop*HcamCrop, 0,0);
				WIFI_sendData(pimgPadded, WcamPadded*HcamPadded*3,3 ,&EXTERNAL_RAM);
			
			}
		}
       }

	stopWifi();
	
	// Network Destructor
	AT_DESTRUCT();
	
	DeallocOutput();
	
	pi_cluster_close(&cluster_dev);
	printf("pi_cluster_close");
	pmsis_exit(0);
	printf("fine chiamata pmsis_exit(0)");

	return 0;
   

   }


int main(void)
{
    printf("\n\n\t *** ImageNet classification on GAP ***\n");
    return pmsis_kickoff((void *) body);
}

//--------------------------


