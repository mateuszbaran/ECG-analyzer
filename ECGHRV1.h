#pragma once

#include "ECGSignal.h"

/**
 * @class Class for parameters created in HRV1 module
 */
class ECGHRV1
{
public:

	ECGHRV1();
	~ECGHRV1();

	//time domain parameters
	double RR_avg;
	double RR_stddev;
	double SDNN;
	double RMSSD;
	double NN50;
	double pNN50;
	double SDANN;
	double SDANN_index;
	double SDSD;

	//frequency domain parameters
	double TP;
	double HF;
	double LF;
	double VLF;
	double ULF;
	double LFHF;

	//vectors are needed to plot frequency figure
	OtherSignal freqency;
	OtherSignal power;
};

