#pragma once
#include "ModulesMethods.h"
#include "ModulesInterfaces.h"

class MovingAverage
{
public:
	MovingAverage(){};
	~MovingAverage(){};

	void removeBaseline(ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal, int span);

private:
	double calculateAvgValueOfNeighbours(gsl_vector *signal, int currentIndex, int span);
};