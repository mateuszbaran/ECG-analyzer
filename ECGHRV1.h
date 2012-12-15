#pragma once

#include "ECGSignal.h"
#include "ModulesInterfaces.h"

/**
 * @class Class for parameters created in HRV1 module
 */
class ECGHRV1 : public HRV1Module
{
public:

	ECGHRV1();
	~ECGHRV1();
	void runModule(const ECGRs & r_peaks_data, ECGHRV1 & hrv1_data);
	void run(const ECGRs & r_peaks_data);
	void setParams(ParametersTypes &parameterTypes);

	//parameters
	const ECGRs r_peaks_data;

	//time domain parameters
	double RR_avg;
	double RR_stddev;
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

