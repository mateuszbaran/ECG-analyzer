/// Multiplier for input parameters - 1 = ms, 0.001 = seconds
#define MULTIPLIER 1

/// Enables development mode - defined => development mode, undefined => standard mode
//#define DEV

/// Enables debug mode
#define DEBUG
//#define DEBUG_FFT

#pragma once

#include <cmath>
#include <vector>
#include "fft/kiss_fft.h"
#include "fft/kiss_fftr.h"
#include "spline/interpolation.h"

#include "ECGRs.h"
#include "ECGHRV1.h"
#include "ECGSignal.h"

#ifdef DEV // 4 testing
    #include "ExampleSignal.h"
#else
	#include "ModulesInterfaces.h"
#endif

/**
 * @class Class for parameters created in HRV1 module
 */
#ifndef DEV // 4 testing
	class HRV1Analyzer : public HRV1Module {
#else
	class HRV1Analyzer {
#endif

public:
	HRV1Analyzer();
	~HRV1Analyzer();

	void runModule(const ECGInfo &, const ECGRs &, ECGHRV1 &);

	#ifndef DEV
        void setParams(ParametersTypes &);
    #endif

private:
    //input parameters
	ECGRs rPeaksData;
	ECGHRV1* hrv1Data;

	//RR peaks signal parameters
	double *sig;
	double *sigAbsolute;
    int signalSize;
    int signalSampling;

    //methods
    void prepareSignal();
    void prepareSigAbsolute();
	void calculateParameters();

	kiss_fft_cpx* copycpx(double *mat, int nframe);
	double mean(double *tab, int start, int end);
	double std(double *tab, int start, int end);
	double* cubicSpline(double* x, double* y, int nframe);
};

