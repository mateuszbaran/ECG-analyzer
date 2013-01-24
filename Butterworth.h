#pragma once
#include "ModulesMethods.h"
#include "ModulesInterfaces.h"

class Butterworth
{
public:
	Butterworth();
	~Butterworth();

	std::vector<std::vector<double>> filterDesign(int order, double attenuation, double cutoffFreq, int sampleFreq, int type);
};