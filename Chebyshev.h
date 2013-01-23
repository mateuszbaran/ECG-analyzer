#pragma once
#include "ModulesMethods.h"
#include "ModulesInterfaces.h"
#include "spuc/chebyshev.h"

class Chebyshev
{
public:
	Chebyshev(){};
	~Chebyshev(){};

	SPUC::chebyshev<double> spuc_chebyshev(double cutoff_frequency, long order, double ripple=3);
	//std::vector<std::vector<double>> filterDesign(int order, int cutoffFreq, int sampleFreq, int type);
};