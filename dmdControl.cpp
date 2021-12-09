#include "mex.h"
#include <iostream>
#include "TLDFMX.h"
#include "TLDFM.h"
//#include "TLDFMX_def.h"
//#include "TLDFM_def.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>

#ifdef _CVI_
   #define EXIT_SUCCESS 0
   #define EXIT_FAILURE 1
#endif

#define  MAX_SEGMENTS 40

using namespace std;

ViStatus select_instrument (ViChar **resource);
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {

ViStatus  err = VI_SUCCESS;
ViSession instrHdl = VI_NULL;
ViChar *rscPtr;
ViInt32   zernikeCount, systemMeasurementSteps, relaxSteps;
ViReal64  minZernikeAmplitude, maxZernikeAmplitude, voltage = 50.0;
ViReal64  mirrorPattern[MAX_SEGMENTS];

ViChar    manufacturer[TLDFM_BUFFER_SIZE];
ViChar    instrumentName[TLDFM_MAX_INSTR_NAME_LENGTH];
ViChar    serialNumber[TLDFM_MAX_SN_LENGTH];
ViBoolean deviceAvailable;
ViChar    resourceName[TLDFM_BUFFER_SIZE];

ViUInt32  relaxPart = T_MIRROR;
ViBoolean isFirstStep = VI_TRUE, reload = VI_TRUE;
ViReal64  relaxPattern[MAX_SEGMENTS];
ViInt32   remainingRelaxSteps;

ViReal64 amplitute = 1;
ViReal64 angle = -180.0;
    
for(ViInt32 i = 0; MAX_SEGMENTS > i; ++i)
	{
		mirrorPattern[i] = voltage;
}

ViUInt32 deviceCount = 0;
int      dm_num = 0;
TLDFM_get_device_count(VI_NULL, &deviceCount);
TLDFM_get_device_information(VI_NULL,dm_num, manufacturer, instrumentName,
										   serialNumber, &deviceAvailable, resourceName);
		
cout << "Dispositivos conectados: " << deviceCount << endl;
cout << "Manufacturer: " << manufacturer << endl;
cout << "Instrument Name: " <<instrumentName << endl;
cout << "Serial Numer: " << serialNumber << endl;
select_instrument(&rscPtr);
cout << "Loading ..." << endl;
TLDFMX_init(rscPtr, VI_TRUE, VI_TRUE, &instrHdl);

cout << "Deformable Mirror Conected" << endl;
TLDFMX_get_parameters(instrHdl,	&minZernikeAmplitude, &maxZernikeAmplitude,
								&zernikeCount, &systemMeasurementSteps, &relaxSteps);
cout << "Driver Parameters: " << endl;
cout << "Zernike Amplitude: "<<"Min:" <<minZernikeAmplitude
        << " Max:" << maxZernikeAmplitude << endl;
cout << "Zernike Amount:" << zernikeCount << endl;
cout << "System Measurement Steps:" << systemMeasurementSteps << endl;
cout << "Relax Steps: " << relaxSteps << endl;

cout << "Reset DM" << endl;
TLDFMX_reset(instrHdl);
cout << "Relax Instrument" << endl;
TLDFMX_relax(instrHdl, relaxPart, isFirstStep,
				   reload, relaxPattern, VI_NULL, &remainingRelaxSteps);
TLDFM_set_segment_voltages(instrHdl, relaxPattern);
isFirstStep = VI_FALSE;
int rep = 0;
do{TLDFMX_relax(instrHdl, relaxPart, isFirstStep,
			    reload, relaxPattern, VI_NULL, &remainingRelaxSteps);
		TLDFM_set_segment_voltages(instrHdl, relaxPattern);
   cout << remainingRelaxSteps << endl;
   rep ++;
   if(rep == 100){remainingRelaxSteps = -1;}
  }while(0 < remainingRelaxSteps);

    if(remainingRelaxSteps == -1){
        cout << "Ejecute de nuevo el programa" << endl;
    }else{
        cout << "Cambiando angulo de DM" << endl;
        int repeat = 0;
        while(repeat < 100){    
        TLDFM_set_tilt_amplitude_angle(instrHdl,1.0,angle);
        angle++;
        //cout << angle << endl;
        if(angle == 180){angle = -180; repeat++;}
        }
    }
/*TLDFM_set_tilt_amplitude_angle(instrHdl,1.0,-180.0);
Sleep(100);
TLDFM_set_tilt_amplitude_angle(instrHdl,1.0,0.0);
Sleep(100);
TLDFM_set_tilt_amplitude_angle(instrHdl,1.0,180.0);
Sleep(100);
TLDFM_set_tilt_amplitude_angle(instrHdl,1.0,-180.0);*/




cout << "Closed DM" << endl;
TLDFMX_reset(instrHdl);
TLDFMX_close(instrHdl);

cout << "Compilado con exito" << endl;

}

ViStatus select_instrument (ViChar **resource)
{
	ViStatus err;
	ViUInt32 deviceCount = 0;
	int      choice = 0;
	ViChar    manufacturer[TLDFM_BUFFER_SIZE];
	ViChar    instrumentName[TLDFM_MAX_INSTR_NAME_LENGTH];
    ViChar    serialNumber[TLDFM_MAX_SN_LENGTH];
    ViBoolean deviceAvailable;
    ViChar    resourceName[TLDFM_BUFFER_SIZE];
	TLDFM_get_device_information(VI_NULL,choice, manufacturer,instrumentName,
                                 serialNumber,&deviceAvailable, resourceName);
	if(VI_SUCCESS == err)
	{	*resource = (ViChar*)malloc(TLDFM_BUFFER_SIZE);
		strncpy(*resource, resourceName, TLDFM_BUFFER_SIZE);}
	return err;
}


