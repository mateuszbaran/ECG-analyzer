/// Enables development mode - defined => development mode, undefined => standard mode
//#define DEV 1

#pragma once

#include <cmath>
#include <vector>
#include "fft/kiss_fft.h"
#include "fft/kiss_fftr.h"

#include "ECGRs.h"
#include "ECGHRV1.h"
#include "ECGSignal.h"

#ifndef DEV
	#include "ModulesInterfaces.h"
	#include "ModulesMethods.h"
#endif

#ifdef DEV // 4 testing
    #include "ExampleSignal.h"
#endif

/**
 * @class Class for parameters created in HRV1 module
 */
class HRV1Analyzer : public HRV1Module {

public:
	HRV1Analyzer();
	~HRV1Analyzer();

	void runModule(const ECGRs &, ECGHRV1 &);

	#ifndef DEV
        void setParams(ParametersTypes &);
    #endif

private:
	ECGRs rPeaksData;
	ECGHRV1* hrv1Data;

	double *sig;
    int signalSize;
    int signalSampling;

	void run();
	kiss_fft_cpx* copycpx(double *mat, int nframe);
	double mean(double *tab, int start, int end);
	double std(double *tab, int start, int end);

};

