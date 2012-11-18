#pragma once
#include "ModulesMethods.h"
#include "ModulesInterfaces.h"

class BaselineRemoval : ECGBaselineModule 
{
public:
	BaselineRemoval();
	~BaselineRemoval();

	void runModule(const ECGSignal &, ECGSignal &);
 	void setParams(ParametersTypes &);

private:
	BASELINE_REMOVAL_METHOD baselineRemovalMethod;

	void movingAverageBaselineRemoval(const ECGSignal &inputSignal, ECGSignal &outputSignal, int span);
	double BaselineRemoval::calculateAvgValueOfNeighbours(gsl_vector *signal, int currentIndex, int span);
};
