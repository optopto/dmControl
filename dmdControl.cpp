#include "mex.h"
#include <iostream>
#include "TLDFMX.h"
#include "TLDFM.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <math.h>

#ifdef _CVI_
   #define EXIT_SUCCESS 0
   #define EXIT_FAILURE 1
#endif

#define  MAX_SEGMENTS 40
#define MAX_TILT 3
#define PI 3.14/180
#define delta 0.01

using namespace std;

ViStatus select_instrument (ViChar **resource);
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {

   
ViStatus  err = VI_SUCCESS;
ViSession instrHdl = VI_NULL;
ViChar *rscPtr;
ViInt32   zernikeCount, systemMeasurementSteps, relaxSteps;
ViReal64  minZernikeAmplitude, maxZernikeAmplitude;
ViReal64  mirrorPattern[MAX_SEGMENTS];

ViChar    manufacturer[TLDFM_BUFFER_SIZE];
ViChar    instrumentName[TLDFM_MAX_INSTR_NAME_LENGTH];
ViChar    serialNumber[TLDFM_MAX_SN_LENGTH];
ViBoolean deviceAvailable;
ViChar    resourceName[TLDFM_BUFFER_SIZE];

ViUInt32  relaxPart = T_BOTH;
ViBoolean isFirstStep = VI_TRUE, reload = VI_TRUE;
ViReal64  relaxPattern[MAX_SEGMENTS];
ViReal64  relaxPatternArm[MAX_TILT];
ViInt32   remainingRelaxSteps;

ViReal64 amplitute = 0.0;
ViReal64 theta = 0.0;
ViReal64 angle = -180.0;
ViReal64 alpha = 0.4; 
ViReal64 newAngle = 0.0;
ViReal64 newAmplitute = 0.0;


ViReal64  SegmentVoltages[MAX_SEGMENTS];
ViReal64 zernikeAmplitudes[TLDFMX_MAX_ZERNIKE_TERMS];

zernikeAmplitudes[TLDFMX_Z4_AMPL_POS] = 0.035;
zernikeAmplitudes[TLDFMX_Z5_AMPL_POS] = -0.2;
zernikeAmplitudes[TLDFMX_Z6_AMPL_POS] = 0.02052;
zernikeAmplitudes[TLDFMX_Z7_AMPL_POS] = 0.00;
zernikeAmplitudes[TLDFMX_Z8_AMPL_POS] = 0.00;
zernikeAmplitudes[TLDFMX_Z9_AMPL_POS] = -0.001;
zernikeAmplitudes[TLDFMX_Z10_AMPL_POS] = 0.0;
zernikeAmplitudes[TLDFMX_Z11_AMPL_POS] = 0.0;
zernikeAmplitudes[TLDFMX_Z12_AMPL_POS] = 0.0;
zernikeAmplitudes[TLDFMX_Z13_AMPL_POS] = 0.0;
zernikeAmplitudes[TLDFMX_Z14_AMPL_POS] = 0.00;
zernikeAmplitudes[TLDFMX_Z15_AMPL_POS] = 0.00;

for(int i = 0; i < TLDFMX_MAX_ZERNIKE_TERMS; i ++)
{
cout << zernikeAmplitudes[i] << endl;
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
//TLDFMX_reset(instrHdl);
cout << "Relax Instrument" << endl;
/*TLDFMX_relax(instrHdl, relaxPart, isFirstStep,
				   reload, relaxPattern, relaxPatternArm, &remainingRelaxSteps);
TLDFM_set_voltages(instrHdl, relaxPattern,relaxPatternArm);
isFirstStep = VI_FALSE;
int rep = 0;
do{TLDFMX_relax(instrHdl, relaxPart, isFirstStep,
			    reload, relaxPattern, relaxPatternArm, &remainingRelaxSteps);
		TLDFM_set_voltages(instrHdl, relaxPattern,relaxPatternArm);
   rep ++;
   if(rep == 100){remainingRelaxSteps = -1;}
  }while(0 < remainingRelaxSteps);
*/
    if(remainingRelaxSteps == -1){
        cout << "Ejecute de nuevo el programa" << endl;
    }else{
        //Sleep(3000);
        //182 ms 1 grado
        //92 ms 2 grados
        //73 ms 2.5 grados
        //35 ms 5 grados
         //Z_Ast45_Flag | Z_Def_Flag | Z_Ast0_Flag | Z_TreY_Flag | Z_ComX_Flag     
        //| Z_ComY_Flag| Z_TreX_Flag | Z_TetY_Flag | Z_SAstY_Flag | Z_SAb3_Flag | Z_SAstX_Flag| Z_TetX_Flag
           
        TLDFMX_calculate_zernike_pattern (instrHdl,Z_All_Flag, zernikeAmplitudes, SegmentVoltages);
         TLDFM_set_segment_voltages(instrHdl, SegmentVoltages);
         TLDFM_set_tilt_amplitude_angle(instrHdl,0.0,0.0);
        for(int j = 0; j < MAX_SEGMENTS; j++){
            cout << j << "-" << SegmentVoltages[j] << endl;
        }

        cout << "Cambiando angulo de DM" << endl;
        int repeat = 0;
        
        while(repeat < 10){
        angle = angle + 30;
        newAngle =(180/3.14*atan2(amplitute*sin(PI*theta)+alpha*sin(PI*angle),amplitute*cos(PI*theta)+alpha*cos(PI*angle)));  
        newAmplitute = sqrt(pow(alpha,2)+pow(amplitute,2)+2*amplitute*alpha*cos(PI*(theta - angle)));
        TLDFM_set_tilt_amplitude_angle(instrHdl,alpha,newAngle);
        if(angle == 180){angle = -180; repeat++; cout << repeat << endl;}
        Sleep(500);
        }
    }

cout << "Closed DM" << endl;
TLDFM_set_tilt_amplitude_angle(instrHdl,0.0,0.0);

//TLDFMX_reset(instrHdl);
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


