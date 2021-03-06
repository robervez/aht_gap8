
#if MODEL_ID==99
        #include "modelKernels.h"
#elif MODEL_ID==0
	#include "mobilenet_v1_1_0_224_quantKernels.h"
#elif MODEL_ID==1
	#include "mobilenet_v1_1_0_192_quantKernels.h"
#elif MODEL_ID==2
	#include "mobilenet_v1_1_0_160_quantKernels.h"
#elif MODEL_ID==3
	#include "mobilenet_v1_1_0_128_quantKernels.h"
#elif MODEL_ID==4
	#include "mobilenet_v1_0_75_224_quantKernels.h"
#elif MODEL_ID==5
	#include "mobilenet_v1_0_75_192_quantKernels.h"
#elif MODEL_ID==6
	#include "mobilenet_v1_0_75_160_quantKernels.h"
#elif MODEL_ID==7
	#include "mobilenet_v1_0_75_128_quantKernels.h"
#elif MODEL_ID==8
	#include "mobilenet_v1_0_5_224_quantKernels.h"
#elif MODEL_ID==9
	#include "mobilenet_v1_0_5_192_quantKernels.h"
#elif MODEL_ID==10
	#include "mobilenet_v1_0_5_160_quantKernels.h"
#elif MODEL_ID==11
	#include "mobilenet_v1_0_5_128_quantKernels.h"
#elif MODEL_ID==12
	#include "mobilenet_v1_0_25_224_quantKernels.h"
#elif MODEL_ID==13
	#include "mobilenet_v1_0_25_192_quantKernels.h"
#elif MODEL_ID==14
	#include "mobilenet_v1_0_25_160_quantKernels.h"
#elif MODEL_ID==15
	#include "mobilenet_v1_0_25_128_quantKernels.h"
#elif MODEL_ID==16
	#include "mobilenet_v2_1_4_224_quantKernels.h"
#elif MODEL_ID==17
	#include "mobilenet_v2_1_0_224_quantKernels.h"
#elif MODEL_ID==18
	#include "mobilenet_v2_1_0_192_quantKernels.h"
#elif MODEL_ID==19
	#include "mobilenet_v2_1_0_160_quantKernels.h"
#elif MODEL_ID==20
	#include "mobilenet_v2_1_0_128_quantKernels.h"
#elif MODEL_ID==21
	#include "mobilenet_v2_1_0_96_quantKernels.h"
#elif MODEL_ID==22
	#include "mobilenet_v2_0_75_224_quantKernels.h"
#elif MODEL_ID==23
	#include "mobilenet_v2_0_75_192_quantKernels.h"
#elif MODEL_ID==24
	#include "mobilenet_v2_0_75_160_quantKernels.h"
#elif MODEL_ID==25
	#include "mobilenet_v2_0_75_128_quantKernels.h"
#elif MODEL_ID==26
	#include "mobilenet_v2_0_75_96_quantKernels.h"
#elif MODEL_ID==27
	#include "mobilenet_v2_0_5_224_quantKernels.h"
#elif MODEL_ID==28
	#include "mobilenet_v2_0_5_192_quantKernels.h"
#elif MODEL_ID==29
	#include "mobilenet_v2_0_5_160_quantKernels.h"
#elif MODEL_ID==30
	#include "mobilenet_v2_0_5_128_quantKernels.h"
#elif MODEL_ID==31
	#include "mobilenet_v2_0_5_96_quantKernels.h"
#elif MODEL_ID==32
	#include "mobilenet_v3_large_minimalistic_1_0_224_quantKernels.h"
#elif MODEL_ID==33
	#include "mobilenet_v3_small_1_0_224_quantKernels.h"
#elif MODEL_ID==34
	#include "squeezenetKernels.h"
#endif


//#include "Gap.h"
//#include "gaplib/ImgIO.h"

#ifdef __EMUL__
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/param.h>
#include <string.h>
#endif


