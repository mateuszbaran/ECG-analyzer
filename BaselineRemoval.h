#pragma once
#include "ModulesMethods.h"
#include "ModulesInterfaces.h"

class BaselineRemoval : public ECGBaselineModule 
{
public:
	BaselineRemoval();
	~BaselineRemoval();

	void runModule (const ECGSignal &, const ECGInfo &, ECGSignalChannel &);
 	void setParams(ParametersTypes &);

private:
	BASELINE_REMOVAL_METHOD baselineRemovalMethod;

	void butterworthBaselineRemoval(ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal, const ECGInfo &ecgInfo, int cutoffFreq);
	void movingAverageBaselineRemoval(ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal, int span);
	double calculateAvgValueOfNeighbours(gsl_vector *signal, int currentIndex, int span);
};
