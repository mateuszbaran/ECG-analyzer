#pragma once
#include "ModulesMethods.h"
#include "ModulesInterfaces.h"

class ECGBaselineRemoval : public ECGBaselineModule 
{
public:
	ECGBaselineRemoval();
	~ECGBaselineRemoval();

	void runModule (const ECGSignal &, const ECGInfo &, ECGSignalChannel &);
 	void setParams(ParametersTypes &);

private:
	BASELINE_REMOVAL_METHOD baselineRemovalMethod;
	int span;
	int order;
	double cutoff_frequency;
	double attenuation;
	double ripple;

	void chebyshevBaselineRemoval(ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal, const ECGInfo &ecgInfo, int order, double cutoffFrequency, double attenuation);
	void butterworthBaselineRemoval(ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal, const ECGInfo &ecgInfo, int order, double cutoffFrequency, double ripple);
	void movingAverageBaselineRemoval(ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal, int span);
	double calculateAvgValueOfNeighbours(gsl_vector *signal, int currentIndex, int span);
};
