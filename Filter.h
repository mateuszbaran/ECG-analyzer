#pragma once
#include "ModulesMethods.h"
#include "ModulesInterfaces.h"

class Filter
{
public:
	Filter();
	~Filter();

	void zeroPhase(std::vector<double> b, std::vector<double> a, const ECGSignal &inputSignal, ECGSignal &outputSignal, int order);
	void filter(std::vector<double> b, std::vector<double> a, const ECGSignal &inputSignal, ECGSignal &outputSignal, int order);

};