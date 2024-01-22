#pragma once
#include <stdint.h>

#pragma pack(push, 1)
struct stNewChain
{
	uint32_t	nWorkMode;  	// 0..13 - GRI number 
								// 14..16 - num station in chain
								// 17 - type of gri, '0'-stationary, '1'-mobile
								// 18..21 - station ban All '1'-on, '0'-off
								// 18 - sttaion ban Slave1  '1'-on, '0'-off
								// 19 - station ban Slave2  '1'-on, '0'-off
								// 20 - station ban Slave3  '1'-on, '0'-off
								// 21 - station ban Slave4  '1'-on, '0'-off							
								// 22 - save chain '1' - ROM, '0' - RAM
								// 23 - delete chain '1'- delete, '0' - not delete
								// 24..31 reserve
								
	uint32_t	Cp;				// speed of radio waves
	int32_t		fB[5];  		// latitude  stations, scale factor is *(1<<28) radian
	int32_t		fL[5];  		// longitude stations, scale factor is *(1<<28) radian
	uint32_t 	fED[5];  		// array of time delays stations * 1000 mcs

	uint8_t		mName_GRI[50];	//chain name	                                                                            
};
#pragma pack(pop)