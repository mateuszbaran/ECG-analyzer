#pragma once
#include "ModulesMethods.h"
#include "ModulesInterfaces.h"

class Chebyshev
{
public:
	Chebyshev();
	~Chebyshev();

	std::vector<std::vector<double>> filterDesign(int order, double ripple, double cutoffFreq, int sampleFreq, int type);
};